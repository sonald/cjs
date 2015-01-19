// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// Copyright (C) Sian Cao <yinshuiboy@gmail.com>, 2015

#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "debug.h"

#include <iostream>
#include <fstream>
#include <memory>

namespace cjs
{
    using namespace std;
    using namespace ast;
    DEF_DEBUG_FOR(Logger::Parser);
    DEF_ERROR_FOR(Logger::Parser);

    Parser::Parser()
        :_tokens{nullptr}
    {
    }

    using TokenType = Token::TokenType;

    Ast* Parser::program(const string& path)
    {
        unique_ptr<ifstream> ifs(new ifstream {path});
        if (!*ifs) { 
            return nullptr;
        }

        this->_tokens.reset(new Tokenizer{move(ifs)});
        auto* nd = new Program;
        do {
            nd->append(expressionStatement());
        } while (_tokens->peek() != TokenType::EOS);
        return nd;
    }

    Ast* Parser::expressionStatement()
    {
        debug("parse expr statement");
        auto* nd = new ExpressionStatement {expression(1)};
        _tokens->match(Token::TokenType::Semicolon);
        debug("parsed expr statement");
        return nd;
    }

    Ast* Parser::primary()
    {
        switch(_tokens->peek()) {
            case TokenType::StringLiteral:
            case TokenType::BooleanLiteral:
            case TokenType::NullLiteral:
            case TokenType::NumericLiteral:
            case TokenType::RegexLiteral:
                return literal();

            case TokenType::Identifier:
                return ident();

            default:
                error("parse primary failed");
        }
        return nullptr;
    }

    Ast* Parser::literal()
    {
        debug("parse literal");
        AstType ty;
        Token tk = _tokens->token();
        switch(_tokens->peek()) {
            case TokenType::StringLiteral:
                ty = AstType::StringLiteral;
                _tokens->match(TokenType::StringLiteral);
                break;

            case TokenType::BooleanLiteral:
                ty = AstType::BooleanLiteral;
                _tokens->match(TokenType::BooleanLiteral);
                break;

            case TokenType::NullLiteral:
                ty = AstType::NullLiteral;
                _tokens->match(TokenType::NullLiteral);
                break;

            case TokenType::NumericLiteral:
                ty = AstType::NumericLiteral;
                _tokens->match(TokenType::NumericLiteral);
                break;

            case TokenType::RegexLiteral:
                ty = AstType::RegularExpressionLiteral;
                _tokens->match(TokenType::RegexLiteral);
                break;

            default:
                return nullptr;
                break;
        }

        Ast* nd = new Literal(tk, ty);
        return nd;
    }

    Ast* Parser::ident()
    {
        debug("parse ident");
        auto* nd = new Identifier {_tokens->token()};
        _tokens->match(TokenType::Identifier);
        return nd;
    }

    ast::Ast* Parser::callexpr(ast::Ast* lhs)
    {
        auto* nd = new CallExpression {lhs, args()};
        return nd;
    }

    Ast* Parser::memberexpr(Ast* obj)
    {
        if (obj) {
            if (_tokens->peek() != TokenType::Dot) {
                if (obj->type() == ast::AstType::MemberExpression)
                    return obj;
                else 
                    return new MemberExpression {obj};
            }

            _tokens->match(TokenType::Dot);
            Ast* p = primary();
            auto* nd = new MemberExpression {obj, p};
            return memberexpr(nd);

        } else {
            Ast* p = primary();
            return memberexpr(p);
        }
    }

    Ast* Parser::args()
    {
        auto* nd = new CallArgs;
        _tokens->match(TokenType::LParen); // eat '('
        if (_tokens->peek() != TokenType::RParen) {
            while (1) {
                nd->append(primary());
                if (_tokens->peek() != TokenType::Comma)
                    break;
                _tokens->match(TokenType::Comma);
            }
        }
        _tokens->match(TokenType::RParen); // eat ')'

        return nd;
    }

    ast::Ast* Parser::expression(int rbp)
    {
        auto* left = nud(_tokens->peek());
        auto op = _tokens->token();
        while (pred(op.type, true) >= rbp) {
            left = led(op, left);
            op = _tokens->token();
        }
        return left;
    }

    int Parser::pred(TokenType ty, bool post)
    {
        switch(ty) {
            case TokenType::EOS: return 0;
            case TokenType::ASSIGNMENT: return 10;
            case TokenType::PLUS: return post ? 40: 20;
            case TokenType::MINUS: return post ? 40: 20;
            case TokenType::MUL: return 30;
            case TokenType::DIV: return 30;
            case TokenType::INCREMENT: return post ? 50: 40;
            case TokenType::DECREMENT: return post ? 50: 40;
            case TokenType::NEW: return 60;
            case TokenType::LParen: return 70; // callexpr
            case TokenType::Dot: return 80; // member ref
            default: return 0;
        }
        return 0;
    }

    Parser::Associate Parser::assoc(TokenType ty)
    {
        if (ty == TokenType::ASSIGNMENT) return Associate::RightAssoc;
        if (ty == TokenType::NEW) return Associate::RightAssoc;
        else return Associate::LeftAssoc;
    }

    ast::Ast* Parser::led(const Token& tk, ast::Ast* left)
    {
        _tokens->advance();
        ast::Ast* ret = nullptr;
        switch (tk.type) {
            case TokenType::ASSIGNMENT:
            {
                debug("parse assignment");
                //FIXME: right assoc
                ast::Ast* assignee = expression(pred(tk.type));
                ret = new AssignExpression(left, assignee);
                break;
            }

            // infix
            case TokenType::MINUS:
            case TokenType::PLUS:
            {
                debug("parse additive");
                ast::Ast* rhs = expression(pred(tk.type));
                ret = new AdditiveExpression(tk, left, rhs);
                break;
            }

            case TokenType::MUL:
            case TokenType::DIV:
            {
                debug("parse multitive");
                ast::Ast* rhs = expression(pred(tk.type));
                ret = new MultitiveExpression(tk, left, rhs);
                break;
            }

            // postfix
            case TokenType::DECREMENT:
            case TokenType::INCREMENT:
            {
                debug("parse postfix");
                ret = new PostfixExpression(tk, left);
                break;
            }

            case TokenType::Dot:
            {
                debug("parse memberexpr");
                Ast* p = expression(pred(tk.type));
                ret = new MemberExpression{left, p};
                break;
            }

            case TokenType::LParen:
            {
                debug("parse callexpr");
                if (left->type() != ast::AstType::MemberExpression)
                        left = new MemberExpression{left};
                ret = callexpr(left);
                break;
            }

            default:
                error("nud is not valid for (%)", tk);
                break;
        }
        return ret;
    }

    ast::Ast* Parser::nud(TokenType ty)
    {
        Token op = _tokens->token();
        ast::Ast* ret = nullptr;
        switch (ty) {
            //unary and prefix
            case TokenType::MINUS:
            case TokenType::PLUS:
            case TokenType::DECREMENT:
            case TokenType::INCREMENT:
            {
                debug("parse unary");
                _tokens->advance();
                ast::Ast* unary = expression(pred(op.type, false));
                ret = new UnaryExpression(op, unary);
                break;
            }

            case TokenType::NEW:
            {
                debug("parse new expr");
                _tokens->advance();
                ast::Ast* member = expression(pred(op.type));
                ret = new NewExpression(member);
                break;
            }

            case TokenType::StringLiteral:
            case TokenType::BooleanLiteral:
            case TokenType::NullLiteral:
            case TokenType::NumericLiteral:
            case TokenType::RegexLiteral:
            {
                ret = literal();
                break;
            }

            case TokenType::Identifier:
            {
                ret = ident();
                break;
            }

            default:
                error("nud is not valid for (%s)", op.sval);
                break;
        }
        return ret;
    }
}

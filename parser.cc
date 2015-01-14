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

using namespace std;
namespace cjs
{
    using namespace ast;
    DEF_DEBUG_FOR(Logger::Parser);

    static void error(const string& msg)
    {
        cerr << "Parser: " << msg << endl;
        exit(-1);
    }

    void ReprVisitor::visit(ast::Program* node)
    {
        for(auto& ptr: node->statements()) {
            ptr->visit(this);
        }
    }

    void ReprVisitor::visit(ast::ExpressionStatement* node)
    {
        node->expression()->visit(this);
        _repr += ";";
    }

    void ReprVisitor::visit(ast::Expression* node)
    {
    }

    void ReprVisitor::visit(ast::CallExpression* node)
    {
        node->callee()->visit(this);
        node->args()->visit(this);
    }

    void ReprVisitor::visit(ast::MemberExpression* node)
    {
        node->object()->visit(this);
        if (node->property()) {
            _repr += ".";
            node->property()->visit(this);
        }
    }

    void ReprVisitor::visit(ast::CallArgs* node)
    {
        bool f = true;
        auto& l = node->args();
        _repr += "(";
        for_each(l.begin(), l.end(), [&](const ast::AstPtr& ptr) {
            if (!f) _repr += ".";
            ptr->visit(this);
            f = false;
        });
        _repr += ")";
    }

    void ReprVisitor::visit(ast::Identifier* node)
    {
        _repr += node->token().sval;
    }

    void ReprVisitor::visit(ast::StringLiteral* node)
    {
        _repr += "\'" + node->token().sval + "\'";
    }


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
        auto* nd = new ExpressionStatement {expr()};
        _tokens->match(Token::TokenType::Semicolon);
        return nd;
    }

    Ast* Parser::expr()
    {
        return callexpr();
    }

    Ast* Parser::primary()
    {
        if (_tokens->peek() == TokenType::StringLiteral) {
            return literal();

        } else if (_tokens->peek() == TokenType::Identifier) {
            return ident();

        } else 
            error("parse primary failed");
        return nullptr;
    }
    Ast* Parser::literal()
    {
        auto* nd = new StringLiteral(_tokens->token());
        _tokens->match(TokenType::StringLiteral);
        return nd;
    }

    Ast* Parser::ident()
    {
        auto* nd = new Identifier {_tokens->token()};
        _tokens->match(TokenType::Identifier);
        return nd;
    }

    Ast* Parser::callexpr()
    {
        auto* nd = new CallExpression {memberexpr(), args()};
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

}

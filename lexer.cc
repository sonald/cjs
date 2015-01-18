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

#include "lexer.h"
#include "debug.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <regex>

using namespace std;
namespace cjs
{
    DEF_DEBUG_FOR(Logger::Tokenizer);
    DEF_ERROR_FOR(Logger::Tokenizer);

    using TType = Token::TokenType;
    unordered_map<Token::TokenType, string> typeNames {
        {TType::FUNCTION, "FUNCTION"},
        {TType::VAR,  "VAR"},
        {TType::BREAK,   "BREAK"},
        {TType::DO,  "DO"},
        {TType::INSTANCEOF,  "INSTANCEOF"},
        {TType::TYPEOF,"TYPEOF"},
        {TType::CASE,    "CASE"},
        {TType::ELSE,    "ELSE"},
        {TType::NEW, "NEW"},
        {TType::CATCH,   "CATCH"},
        {TType::FINALLY, "FINALLY"},
        {TType::RETURN,  "RETURN"},
        {TType::VOID,"VOID"},
        {TType::CONTINUE,    "CONTINUE"},
        {TType::FOR, "FOR"},
        {TType::SWITCH,  "SWITCH"},
        {TType::WHILE,"WHILE"},
        {TType::DEBUGGER,    "DEBUGGER"},
        {TType::THIS,    "THIS"},
        {TType::WITH,"WITH"},
        {TType::DEFAULT, "DEFAULT"},
        {TType::IF,  "IF"},
        {TType::THROW,   "THROW"},
        {TType::DELETE,  "DELETE"},
        {TType::IN,  "IN"},
        {TType::TRY,"TRY"},
        {TType::Identifier,"Identifier"},
        {TType::Semicolon,"Semicolon"},
        {TType::Comma,"Comma"},
        {TType::Dot,"Dot"},
        {TType::LBrace,"LBrace"},
        {TType::RBrace,"RBrace"},
        {TType::LParen,"LParen"},
        {TType::RParen,"RParen"},
        {TType::ASSIGNMENT,"ASSIGNMENT"},
        {TType::PLUS,"PLUS"},
        {TType::MINUS,"MINUS"},
        {TType::MUL,"MUL"},
        {TType::DIV,"DIV"},
        {TType::UNARY_PLUS,"UNARY_PLUS"},
        {TType::UNARY_MINUS,"UNARY_MINUS"},
        {TType::INCREMENT,"INCREMENT"},
        {TType::DECREMENT,"DECREMENT"},
        {TType::SingleQuote,"SingleQuote"},
        {TType::StringLiteral,"StringLiteral"},
        {TType::NullLiteral,"NullLiteral"},
        {TType::BooleanLiteral,"BooleanLiteral"},
        {TType::NumericLiteral,"NumericLiteral"},
        {TType::RegexLiteral,"RegexLiteral"},
        {Token::TokenType::EOS,"EOS"},
    };

    ostream& operator<<(ostream& os, const Token& tk)
    {
        return os << "Token{" << typeNames[tk.type] << ": " << tk.sval << "}";
    }

    Tokenizer::Tokenizer(unique_ptr<ifstream> is)
        :_is {move(is)}
    {
        advance();
    }

    Tokenizer::~Tokenizer()
    {
    }

    Token::TokenType Tokenizer::peek()
    {
        return _next.type;
    }
    
    Token Tokenizer::token()
    {
        return _next;
    }

    void Tokenizer::unget()
    {
        _ungets.push(_next);
    }

    void Tokenizer::skipWhite() {
        if (_state == State::InStart) {
            int ch;
            while (*_is && (isspace(ch = _is->get()))) {
                if (ch == '\n') _lines++;
            }
            _is->unget();
        }
    }

    void Tokenizer::advance()
    {
        if (_ungets.size()) {
            _next = _ungets.top();
            _ungets.pop();
            return;
        }

        unordered_map<int, Token::TokenType> puncts {
            {';', Token::TokenType::Semicolon},
            {',', Token::TokenType::Comma},
            {'(', Token::TokenType::LParen},
            {')', Token::TokenType::RParen},
            {'{', Token::TokenType::RBrace},
            {'}', Token::TokenType::RBrace},
            {'.', Token::TokenType::Dot},
            {'=', Token::TokenType::ASSIGNMENT},
            {'*', Token::TokenType::MUL},
            {'/', Token::TokenType::DIV},
            // ambiguous
            {'-', Token::TokenType::MINUS},
            {'+', Token::TokenType::PLUS},
        };

        skipWhite();

        if (!*_is) {
            _next.type = Token::TokenType::EOS;
            return;
        }

        int ch = _is->get();

        if (ch == '/') {
            if (_is->get() == '*') {
                parseComment();
                skipWhite();
                ch = _is->get();
            } else {
                _is->unget(); // DIV
            }
        } 
        
        if (!*_is) {
            _next.type = Token::TokenType::EOS;
            return;
        }

        if (ch == '"') {
            parseString();

        } else if (puncts.find(ch) != puncts.end()) {
            _next.type = puncts[ch];
            _next.sval = {(char)ch};

            if (ch == '+') {
                if (_is->get() == '+') {
                    _next.type = Token::TokenType::INCREMENT;
                    _next.sval = "++";
                } else _is->unget();
            } else if (ch == '-') {
                if (_is->get() == '-') {
                    _next.type = Token::TokenType::DECREMENT;
                    _next.sval = "--";
                } else _is->unget();
            } 

            debug("parsing puncts (%)", _next.sval);

        } else if (ch >= '0' && ch <= '9') {
            _is->unget();
            parseNumeric();

        } else { // ident
            _is->unget();
            parseIdent();
        }
    }

    void Tokenizer::parseRegExp()
    {
        debug("% not implemented.", __func__);
    }

    void Tokenizer::parseString()
    {
        int ch;
        debug("parsing literal");
        string sval;
        while ((*_is) && (ch = _is->get()) != '"') {
            sval += ch;
        }

        if (ch != '"') {
            error("Tokenizer: parse error");
        }

        _next.type = Token::TokenType::StringLiteral;
        _next.sval = sval;
        debug("parsed literal (%)", sval);
    }

    void Tokenizer::parseIdent()
    {
        unordered_map<string, Token::TokenType> ty {
            {"undefined", Token::TokenType::Identifier},
            {"null", Token::TokenType::NullLiteral},
            {"true", Token::TokenType::BooleanLiteral},
            {"false", Token::TokenType::BooleanLiteral},

            {"function", Token::TokenType::FUNCTION},
            {"var", Token::TokenType::VAR},
            {"break",    Token::TokenType::BREAK},
            {"do",   Token::TokenType::DO},
            {"instanceof",   Token::TokenType::INSTANCEOF},
            {"typeof", Token::TokenType::TYPEOF},
            {"case",     Token::TokenType::CASE},
            {"else",     Token::TokenType::ELSE},
            {"new",  Token::TokenType::NEW},
            {"catch",    Token::TokenType::CATCH},
            {"finally",  Token::TokenType::FINALLY},
            {"return",   Token::TokenType::RETURN},
            {"void", Token::TokenType::VOID},
            {"continue",     Token::TokenType::CONTINUE},
            {"for",  Token::TokenType::FOR},
            {"switch",   Token::TokenType::SWITCH},
            {"while", Token::TokenType::WHILE},
            {"debugger",     Token::TokenType::DEBUGGER},
            {"this",     Token::TokenType::THIS},
            {"with", Token::TokenType::WITH},
            {"default",  Token::TokenType::DEFAULT},
            {"if",   Token::TokenType::IF},
            {"throw",    Token::TokenType::THROW},
            {"delete",   Token::TokenType::DELETE},
            {"in",   Token::TokenType::IN},
            {"try", Token::TokenType::TRY},
        };

        int ch = _is->get();
        debug("parsing Identifier ");
        string sval;
        if (isalpha(ch) || ch == '_') {
            sval += ch;
            do {
                ch = _is->get();
                if (isalnum(ch) || ch == '_') {
                    sval += ch;
                } else {
                    _is->unget();
                    debug("unget (%)", (char)ch);
                    break; 
                }
            } while (1);

            if (ty.find(sval) != ty.end()) {
                _next.type = ty[sval];
            } else // *undefined* is also identifier
                _next.type = Token::TokenType::Identifier;

            _next.sval = sval;
            debug("parsed Identifier (%)", sval);

        } else {
            error("parse ident error");
        }
    }

    void Tokenizer::parseNumeric()
    {
        static regex reNum{ "(?:0|[1-9]\\d*)(?:(\\.\\d*)?((e|E)\\d+)?)?" };
        int ch;
        string val;
        while (*_is) {
            ch = _is->get();
            if ((ch >= '0' && ch <= '9') || ch == '.' || ch == 'e' || ch == 'E') {
                val += ch;
            } else break;
        }

        if (std::regex_match(val, reNum)) {
            _next.type = Token::TokenType::NumericLiteral;
            _next.sval = val;
        } else 
            error("parse numeric error (%)", val);
        _is->unget();
    }

    void Tokenizer::parseComment()
    {
        int ch;
        debug("parsing comment");
        bool malformed = true;
        int pre = 0;
        while (*_is) {
            ch = _is->get();
            if (ch == '\n') _lines++;
            if (pre == '*' && ch == '/') {
                malformed = false;
                break;
            }
            pre = ch;
        }

        if (malformed) error("parse comment failed");
    }


    bool Tokenizer::match(const Token::TokenType& tt)
    {
        if (_next.type == tt) {
            advance();
            return true;
        }
        return false;
    }
}

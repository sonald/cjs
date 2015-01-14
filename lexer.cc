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

/**
 * literal := ".*"
 * ident := [:alpha:_][:alnum:_]*
 *
 */

using namespace std;
namespace cjs
{
    DEF_DEBUG_FOR(Logger::Tokenizer);

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
            {'.', Token::TokenType::Dot}
        };

        int ch;
        while (*_is && (isspace(ch = _is->get()))) {
            if (ch == '\n') _lines++;
        }

        if (!*_is) {
            _next.type = Token::TokenType::EOS;
            return;
        }

        if (ch == '/') {
            if (_is->get() == '*') {
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
                while (*_is && (isspace(ch = _is->get()))) {
                    if (ch == '\n') _lines++;
                }
            } else {
                error("parse error");
            }
        } 
        
        if (ch == '"') {
            debug("parsing literal");
            string sval;
            while ((*_is) && (ch = _is->get()) != '"') {
                sval += ch;
            }

            if (_is->eof()) {
                error("Tokenizer: parse error");
            }

            _next.type = Token::TokenType::StringLiteral;
            _next.sval = sval;
            debug("parsed literal (%)", sval);

        } else if (puncts.find(ch) != puncts.end()) {
            _next.type = puncts[ch];
            _next.sval = {(char)ch};
            debug("parsing puncts (%)", _next.sval);

        } else { // ident
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
                _next.type = Token::TokenType::Identifier;
                _next.sval = sval;
                debug("parsed Identifier (%)", sval);

            } else {
                error("parse ident error");
            }
        }
    }

    bool Tokenizer::match(const Token::TokenType& tt)
    {
        if (_next.type == tt) {
            advance();
            return true;
        }
        return false;
    }

    void Tokenizer::error(const string& msg)
    {
        cerr << "Tokenizer: error (#" << _lines << "L) " << msg << endl;
        exit(-1);
    }
}

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

#ifndef _CJS_LEXER_H
#define _CJS_LEXER_H

#include <string>
#include <memory>
#include <stack>

namespace cjs 
{
    using namespace std;

    struct Token 
    {
        enum class TokenType: int
        {
            /* keywords */
            FUNCTION,
            VAR,
            BREAK,   
            DO,  
            INSTANCEOF,  
            TYPEOF,
            CASE,    
            ELSE,    
            NEW, 
            CATCH,   
            FINALLY, 
            RETURN,  
            VOID,
            CONTINUE,    
            FOR, 
            SWITCH,  
            WHILE,
            DEBUGGER,    
            THIS,    
            WITH,
            DEFAULT, 
            IF,  
            THROW,   
            DELETE,  
            IN,  
            TRY,

            /* puncts */
            Identifier,
            Semicolon,
            Comma,
            Dot,
            LBrace,
            RBrace,
            LParen,
            RParen,

            ASSIGNMENT,

            PLUS,
            MINUS,

            MUL,
            DIV,

            UNARY_PLUS,
            UNARY_MINUS,

            INCREMENT, // ++
            DECREMENT, // ++

            SingleQuote,
            StringLiteral,
            NullLiteral,
            BooleanLiteral,
            NumericLiteral,
            RegexLiteral,
            EOS
        };

        TokenType type;
        string sval; // for string literal and ident
        friend ostream& operator<<(ostream& os, const Token& tk);
    };

    
    class Tokenizer 
    {
        public:
            Tokenizer(unique_ptr<ifstream> is);
            ~Tokenizer();

            Token::TokenType peek();
            Token token();
            void advance();
            void unget();
            bool match(const Token::TokenType& tt);

        private:
            unique_ptr<ifstream> _is;
            int _lines {1};
            int _col {0};
            Token _next;
            stack<Token> _ungets; // unget buffer

            enum class State {
                InStart,
                InString,
                InRegExp,
            };

            State _state {State::InStart};

            void skipWhite();
            void parseRegExp();
            void parseString();
            void parseNumeric();
            void parseComment();
            void parseIdent();
    };
};

#endif


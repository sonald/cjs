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

#ifndef _CJS_ENV_H
#define _CJS_ENV_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "lexer.h"

namespace cjs
{
    using namespace std;
    using TokenType = Token::TokenType;

    enum class SymbolType {
        External,
        StringLabel,
    };

    struct Symbol {
        SymbolType type;
        string name;
        //FIXME: ugly
        void* val; // should be cast according to type

        Symbol(SymbolType st, string name, void* v)
            :type{st}, name{name}, val{v} 
        {}
    };

    using SymbolPtr = shared_ptr<Symbol>;
    class Environment
    {
        public:
            static Environment* top();
            static void push();
            static void pop();
            static Environment* current();

            Environment(Environment* parent = nullptr);
            void add(string symbol, SymbolType st, void* val);
            SymbolPtr get(string symbol);
            const unordered_map<string, SymbolPtr>& symbols() const
            {
                return _symbols;
            }

        private:
            unordered_map<string, SymbolPtr> _symbols;
            Environment* _parent;
    };

};

#endif



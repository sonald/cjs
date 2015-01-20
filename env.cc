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

#include "env.h"
#include "debug.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>

using namespace std;
namespace cjs
{
    DEF_DEBUG_FOR(Logger::Env);
    const void error(const string& msg)
    {
        cerr << "Env: " << msg << endl;
        exit(-1);
    }

    static Environment* _top_env = nullptr;
    static Environment* _current_env = _top_env;

    Environment* Environment::top()
    {
        if (!_top_env) {
            _top_env = new Environment;
            _current_env = _top_env;
        }
        return _top_env;
    }

    void Environment::push()
    {
        if (!_current_env) top();
        _current_env = new Environment(_current_env);
    }

    void Environment::pop()
    {
        assert(_current_env != nullptr);
        if (_current_env) {
            _current_env = _current_env->_parent;
        }
    }
    
    Environment* Environment::current()
    {
        if (!_current_env) return top();
        return _current_env;
    }

    Environment::Environment(Environment* parent)
        :_parent {parent}
    {
    }

    void Environment::add(string symbol, SymbolType st, SymbolScope scope, void* val)
    {
        debug("add symbol % from %", symbol, this);
        if (_symbols.find(symbol) == _symbols.end())
            _symbols[symbol] = make_shared<Symbol>(st, scope, symbol, val);
        else 
            error("redefiniton of " + symbol);
    }

    SymbolPtr Environment::get(string symbol)
    {
        debug("get symbol % from %", symbol, this);
        if (_symbols.find(symbol) == _symbols.end())
            return {};
        return _symbols[symbol];
    }

};


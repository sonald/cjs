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

#include "debug.h"

namespace cjs
{
    static string tolower(const string& s) 
    {
        string v(s);
        std::transform(v.begin(), v.end(), v.begin(), [](char c) {
            return std::tolower(c);
        });
        return v;
    }

    template <class T, class S>
    static ostream& operator<<(ostream& os, const unordered_map<T,S>& m)
    {
        os << "{";
        for (auto& x: m) {
            os << "(" << x.first << " -> " << x.second << ") ";
        }
        return os << "}";
    }

    Logger log;
    Logger::Logger()
    {
        _domains = {
            {Tokenizer, "Tokenizer"},
            {Parser, "Parser"},
            {TypeChecker, "TypeChecker"},
            {Emitter, "Emitter"},
            {Env, "Env"},
            {Stage, "Stage"},
            {Backend, "Backend"},
        };

        const char* v = std::getenv("CJS_DEBUG");
        if (!v) v = "all";
        string s(tolower(v));

        if (s == "all") {
            for (auto& x: _domains) {
                _enabled[tolower(x.second)] = true;
            }
        } else {
            auto i = s.begin();
            while (i != s.end()) {
                auto p = find(i, s.end(), ',');
                auto dom = s.substr(i-s.begin(), p-i);
                _enabled[dom] = true;
                if (p == s.end()) break;
                i = p+1;
            }
        }

        cerr << "Domains enabled: " << _enabled << endl;
    }

    bool Logger::on(Domain dom)
    {
        return _enabled[tolower(_domains[dom])];
    }
};

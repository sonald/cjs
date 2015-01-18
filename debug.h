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

#ifndef _CJS_DEBUG
#define _CJS_DEBUG 

#include <iostream>
#include <unordered_map>

namespace cjs
{
    using namespace std;
    class Logger {
        public:
            enum Domain {
                Tokenizer,
                Parser,
                TypeChecker,
                Env,
                Emitter,
                Stage,
                Backend,
            };

            Logger();

            template <class... Args>
            void error(Domain dom, const char* fmt, Args... args) 
            {
                cerr << _domains[dom] << ": ";
                debug(fmt, args...);
                exit(-1);
            }

            template <class... Args>
            void debug(Domain dom, const char* fmt, Args... args) 
            {
                cerr << _domains[dom] << ": ";
                debug(fmt, args...);
            }

        private:
            template <class T, class... Args>
            void debug(const char* fmt, T arg, Args... args) { 
                while (*fmt) {
                    if (*fmt == '%') {
                        cerr << arg;
                        debug(fmt+1, args...);
                        return;
                    } 
                    cerr << *fmt;
                    fmt++;
                }
            }

            void debug(const char* fmt) { 
                cerr << fmt << endl; 
            }

        private:
            unordered_map<Domain, string> _domains;
    };

#define DEF_DEBUG_FOR(domain) \
    template <class... Args> \
    static void debug(const char* fmt, Args... args)  \
    { log.debug(domain, fmt, args...); }

#define DEF_ERROR_FOR(domain) \
    template <class... Args> \
    static void error(const char* fmt, Args... args)  \
    { log.error(domain, fmt, args...); }

    extern Logger log;
};

#endif

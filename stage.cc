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

#include "stage.h"

#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "nasm-emitter.h"
#include "backend.h"
#include "debug.h"

#include <iostream>
#include <unistd.h>

namespace cjs 
{
    DEF_DEBUG_FOR(Logger::Stage);
    void Stage::setBackend(Backend* be)
    {
        _be.reset(be);
    }

    void Stage::start()
    {
        for (auto& ptr: _ir) {
            // post runs on destructor
            char tmp[] = "/tmp/cjs_tmp_XXXXXX";
            mktemp(tmp);
            string name = string(tmp) + ".s";
            debug("emitting (%)", name);
            ofstream os {name};
            NasmEmitterVisitor emitter {os};
            ptr->visit(&emitter);

            _be->compile(name);
        }

        _be->link();
        //TODO: cleanup
    }

    void Stage::addCompilationUnit(const string& src)
    {
        Parser parser;
        auto* ast = parser.program(src);

        ReprVisitor repr;
        ast->visit(&repr);
        cout << "ast: " << repr.repr() << endl;

        // build symtab and type-check
        TypeCheckVisitor tc;
        ast->visit(&tc);

        _ir.emplace_back(ast);
    }
};


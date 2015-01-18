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

#include "nasm-emitter.h"
#include "debug.h"
#include "env.h"

#include <algorithm>
#include <cassert>

namespace cjs
{
    using namespace std;
    DEF_DEBUG_FOR(Logger::Emitter);

    NasmEmitterVisitor::NasmEmitterVisitor(ofstream& os)
        :AstVisitor(), _os(os)
    {
    }

    NasmEmitterVisitor::~NasmEmitterVisitor()
    {
    }

    void NasmEmitterVisitor::pre()
    {
        Environment& global = *Environment::top();
        assert(Environment::top() == Environment::current());

        for (const auto& x: global.symbols()) {
            if (x.second->type == SymbolType::External) {
                _os << "    extern _" << x.second->name;
            }
        }

        _os << R"(
    section .text
    global  _main
_main: 
    enter 0, 16
)";

    }

    void NasmEmitterVisitor::post()
    {
        _os << R"(
    mov eax, 0
    leave
    ret
)";
        Environment& top = *Environment::top();
        _os << "section .data" << endl;
        for (auto& ptr: top.symbols()) {
            auto sym = ptr.second.get();
            if (sym->type == SymbolType::StringLabel) {
                string label = sym->name;
                _os << label << ":\n";
                _os << "    db \"" << static_cast<Token*>(sym->val)->sval << "\", 0\n";
            }
        }
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::Program* node) 
    {
        if (phase == AstVisitor::Phase::Capture) {
            pre();
        } else {
            post();
            _os.flush();
        }
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::ExpressionStatement* node) 
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::Expression* node) 
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::CallExpression* node) 
    {
        //can only handle one level of function call right now
        if (phase == AstVisitor::Phase::Bubble) {
            auto* obj = dynamic_cast<ast::MemberExpression*>(node->callee().get());
            auto func = obj->object();
            ast::Identifier* id = dynamic_cast<ast::Identifier*>(func.get());
            auto psym = Environment::current()->get(id->token().sval);
            _os << "call _" << psym->name << endl;
        }
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::MemberExpression* node) 
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::CallArgs* node) 
    {
        if (phase == AstVisitor::Phase::Capture) return;

        auto& l = node->args();
        //x86-64 calling convention
        static string reqs[] = {
            "rdi", "rsi", "rdx", "rcx", "r8", "r9"
        };
        int nr_arg = 0;
        for_each(l.begin(), l.end(), [&](const ast::AstPtr& ptr) {
            if (nr_arg > 5) {
                // push into stack
            }

            if (ptr->type() == ast::AstType::Identifier) {
                auto* id = dynamic_cast<ast::Identifier*>(ptr.get());
                debug("can not handle ident (%) now", id->token().sval);

            } else { // should be literal
                auto* id = dynamic_cast<ast::Literal*>(ptr.get());
                if (id->type() == ast::AstType::StringLiteral) {
                    string label = id->symbol()->name;
                    _os << "mov r10, " << label << endl;
                    _os << "mov " << reqs[nr_arg] << ", r10" << endl;
                }
            } 
            nr_arg++;
        });
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::Identifier* node) 
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::Literal* node) 
    {

    }

};

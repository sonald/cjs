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
#include "astvisitor.h"

#include <algorithm>
#include <cassert>

namespace cjs
{
    using namespace std;
    using namespace ast;
    DEF_DEBUG_FOR(Logger::Emitter);
    DEF_ERROR_FOR(Logger::Emitter);

    NasmEmitterVisitor::NasmEmitterVisitor(ofstream& os)
        :AstVisitor(), _os(os)
    {
    }

    NasmEmitterVisitor::~NasmEmitterVisitor()
    {
    }

    void NasmEmitterVisitor::pre()
    {
        // as suggested by nasm 64-bit
        emit("    default rel");

        Environment& global = *Environment::top();
        assert(Environment::top() == Environment::current());

        for (const auto& x: global.symbols()) {
            if (x.second->scope == SymbolScope::External) {
                emit("    extern", x.second->name);
            }
            if (x.second->scope == SymbolScope::Global) {
                emit("    global ", x.second->name);
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
    mov rax, r10
    leave
    ret
)";
        Environment& top = *Environment::top();
        _os << "section .data" << endl;
        for (auto& ptr: top.symbols()) {
            auto sym = ptr.second.get();
            switch (sym->type) {
                case SymbolType::StringLabel:
                    emit(sym->name, ":");
                    emit("    db \"" + *static_cast<string*>(sym->val) + "\", 0");
                    break;
                case SymbolType::Numeric:
                    emit(sym->name, ":");
                    emit("    dq 0");

                default: break;
            } 
        }
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::Program* node) 
    {
        if (phase == AstVisitor::Phase::Capture) {
            pre();
        } else if (phase == AstVisitor::Phase::Bubble) {
            post();
            _os.flush();
        }
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::ExpressionStatement* node) 
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::AssignExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            if (node->lhs()->type() == AstType::NewExpression) {
                debug("assign for NewExpression not impl");
            } else if (node->lhs()->type() == AstType::CallExpression) {
                debug("assign for CallExpression not impl");
            }
        }

        if (phase == AstVisitor::Phase::Bubble) {
            if (node->lhs()->type() == AstType::Identifier) {
                auto* id = dynamic_cast<ast::Identifier*>(node->lhs().get());
                auto& env = *Environment::current();
                auto sym = env.get("_" + id->token().sval);
                assert(sym);
                emit("mov qword [" + sym->name + "], r10");
            }
        }
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::BinaryExpression* node)
    {
        if (phase == AstVisitor::Phase::Step) {
            emit("push", "r10");
        } else if (phase == AstVisitor::Phase::Bubble) {
            emit("pop", "r11"); // load lhs
            switch (node->op().type) {
                case TokenType::PLUS:
                    emit("add r10, r11"); break;
                case TokenType::MINUS:
                    emit("sub r11, r10"); 
                    emit("mov r10, r11");
                    break;
                case TokenType::MUL:
                    emit("imul r10, r11"); break;
                case TokenType::DIV:
                    emit("push rdx");
                    emit("push rax");
                    emit("xor rdx, rdx"); // clear high bits, not used now
                    emit("mov rax, r11");
                    emit("idiv r10");
                    emit("mov r10, rax");
                    emit("pop rax");
                    emit("pop rdx");
                    break;
                case TokenType::Comma:
                    //nothing, return rhs value as in r10
                    break;
                default: break;
            }
        }
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::UnaryExpression* node)
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::PostfixExpression* node)
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::NewExpression* node)
    {
    }

    void NasmEmitterVisitor::visit(AstVisitor::Phase phase, ast::CallExpression* node) 
    {
        //can only handle one level of function call right now
        if (phase == AstVisitor::Phase::Bubble) {
            auto* obj = dynamic_cast<ast::MemberExpression*>(node->callee().get());
            auto func = obj->object();
            ast::Identifier* id = dynamic_cast<ast::Identifier*>(func.get());
            auto psym = Environment::current()->get("_"+id->token().sval);
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
                    emit("mov r10,", label);
                    emit("mov", reqs[nr_arg],", r10");
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
        if (node->token().type == TokenType::NumericLiteral) {
            emit("mov r10,", node->token().sval);
        }
    }
};

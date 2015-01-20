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

#ifndef _CJS_NASM_EMITTER_H
#define _CJS_NASM_EMITTER_H 

#include "parser.h"
#include "astvisitor.h"
#include <fstream>

namespace cjs
{
    class NasmEmitterVisitor: public AstVisitor
    {
        public:
            NasmEmitterVisitor(ofstream& os);
            ~NasmEmitterVisitor();
            void pre();
            void post();

            virtual void visit(AstVisitor::Phase phase, ast::Program* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::ExpressionStatement* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::AssignExpression* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::BinaryExpression* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::UnaryExpression* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::PostfixExpression* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::NewExpression* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::CallExpression* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::MemberExpression* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::CallArgs* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::Identifier* node) override;
            virtual void visit(AstVisitor::Phase phase, ast::Literal* node) override;

        private:
            ofstream& _os;

            template <class T, class... Args>
            void emit(T arg1, Args... args) {
                _os << arg1 << " ";
                emit(args...);
            }

            void emit() {
                _os << endl;
            }
    };
        
};

#endif


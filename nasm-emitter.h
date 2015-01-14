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

            virtual void visit(ast::Program* node) override;
            virtual void visit(ast::ExpressionStatement* node) override;
            virtual void visit(ast::Expression* node) override;
            virtual void visit(ast::CallExpression* node) override;
            virtual void visit(ast::MemberExpression* node) override;
            virtual void visit(ast::CallArgs* node) override;
            virtual void visit(ast::Identifier* node) override;
            virtual void visit(ast::StringLiteral* node) override;

        private:
            ofstream& _os;
    };
        
};

#endif


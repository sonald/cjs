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

#ifndef _CJS_AST_VISITOR_H
#define _CJS_AST_VISITOR_H

#include <string>

#include "ast.h"

namespace cjs
{
    using namespace std;
    using TokenType = Token::TokenType;

    class AstVisitor
    {
        public:
            enum class Phase {
                Capture,
                Step, // middle phases between child nodes visited
                Bubble
            };

            virtual void visit(Phase phase, ast::Program* node){}
            virtual void visit(Phase phase, ast::ExpressionStatement* node){}
            virtual void visit(Phase phase, ast::Expression* node){}
            virtual void visit(Phase phase, ast::AssignExpression* node){}
            virtual void visit(Phase phase, ast::BinaryExpression* node){}
            virtual void visit(Phase phase, ast::UnaryExpression* node){}
            virtual void visit(Phase phase, ast::PostfixExpression* node){}
            virtual void visit(Phase phase, ast::NewExpression* node){}
            virtual void visit(Phase phase, ast::CallExpression* node){}
            virtual void visit(Phase phase, ast::MemberExpression* node){}
            virtual void visit(Phase phase, ast::CallArgs* node){}
            virtual void visit(Phase phase, ast::Identifier* node){}
            virtual void visit(Phase phase, ast::Literal* node){}
    };

    class ReprVisitor: public AstVisitor
    {
        public:
            virtual void visit(Phase phase, ast::ExpressionStatement* node) override;
            virtual void visit(Phase phase, ast::AssignExpression* node) override;
            virtual void visit(Phase phase, ast::BinaryExpression* node) override;
            virtual void visit(Phase phase, ast::UnaryExpression* node) override;
            virtual void visit(Phase phase, ast::PostfixExpression* node) override;
            virtual void visit(Phase phase, ast::NewExpression* node) override;
            virtual void visit(Phase phase, ast::CallExpression* node) override;
            virtual void visit(Phase phase, ast::MemberExpression* node) override;
            virtual void visit(Phase phase, ast::CallArgs* node) override;
            virtual void visit(Phase phase, ast::Identifier* node) override;
            virtual void visit(Phase phase, ast::Literal* node) override;
            string repr() const { return _repr; }

        private:
            string _repr;
    };
};

#endif



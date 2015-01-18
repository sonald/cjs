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

#include "semantic.h"
#include "debug.h"

#include <cassert>

namespace cjs
{
    DEF_DEBUG_FOR(Logger::TypeChecker)
    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::Program* node) 
    {
        if (phase == AstVisitor::Phase::Capture) {
            Environment& global = *Environment::top();
            global.add("puts", SymbolType::External, nullptr);
        }
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::ExpressionStatement* node) 
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::AssignExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::AdditiveExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::MultitiveExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::UnaryExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::PostfixExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::NewExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::CallExpression* node) 
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::MemberExpression* node) 
    {
        auto func = node->object();
        assert(func->type() == ast::AstType::Identifier);

        ast::Identifier* id = dynamic_cast<ast::Identifier*>(func.get());
        auto psym = Environment::current()->get(id->token().sval);
        assert(!!psym);
        if (!psym) {

        }
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::CallArgs* node) 
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::Identifier* node) 
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::Literal* node) 
    {
        static int next = 0;

        switch(node->type()) {
            case ast::AstType::StringLiteral: 
            {
                string label = "LC" + to_string(next++);
                debug("define local label (%) for (%)", label, node->token().sval);

                // string literal should reside in top env now
                Environment& top = *Environment::top();
                top.add(label, SymbolType::StringLabel, new Token(node->token()));
                node->annotate(label);
                break;
            }

            case ast::AstType::BooleanLiteral:
            case ast::AstType::NumericLiteral:
            case ast::AstType::NullLiteral:
            case ast::AstType::RegularExpressionLiteral:

            default: break;
        }
    }

};


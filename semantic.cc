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
    DEF_ERROR_FOR(Logger::TypeChecker)

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::Program* node) 
    {
        if (phase == AstVisitor::Phase::Capture) {
            Environment& global = *Environment::top();
            global.add("_puts", SymbolType::Function, SymbolScope::External, nullptr);
        }
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::ExpressionStatement* node) 
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::AssignExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            if (node->lhs()->type() == ast::AstType::Identifier) {
                auto* id = dynamic_cast<ast::Identifier*>(node->lhs().get());
                // use top because we have no var declartion now
                // every var goes to global env
                auto& env = *Environment::top();
                string name {"_" + id->token().sval};
                if (env.get(name)) {
                    return;
                }
                //FIXME: need to do type inference bottom up 
                env.add(name, SymbolType::Numeric, SymbolScope::Global, (void*)0);
            }
        }
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::BinaryExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
        }
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::UnaryExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::PostfixExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            if (node->post()->type() != ast::AstType::NewExpression 
                && node->post()->type() != ast::AstType::CallExpression) {
                error("ReferenceError: Invalid left-hand side expression in postfix operation");
            }
        }
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::NewExpression* node)
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::CallExpression* node) 
    {
    }

    void TypeCheckVisitor::visit(AstVisitor::Phase phase, ast::MemberExpression* node) 
    {
        if (phase == AstVisitor::Phase::Bubble) {
            auto func = node->object();
            assert(func->type() == ast::AstType::Identifier);

            ast::Identifier* id = dynamic_cast<ast::Identifier*>(func.get());
            auto psym = Environment::current()->get(id->token().sval);
            //assert(!!psym);
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
                debug("define global label (%) for (%)", label, node->token().sval);

                // string literal should reside in top env now
                Environment& top = *Environment::top();
                top.add(label, SymbolType::StringLabel, SymbolScope::Global, new string(node->token().sval));
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


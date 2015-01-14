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

#include <cassert>

namespace cjs
{
    void TypeCheckVisitor::visit(ast::Program* node) 
    {
        Environment& global = *Environment::top();
        global.add("puts", SymbolType::External, nullptr);
    }

    void TypeCheckVisitor::visit(ast::ExpressionStatement* node) 
    {
    }

    void TypeCheckVisitor::visit(ast::Expression* node) 
    {
    }

    void TypeCheckVisitor::visit(ast::CallExpression* node) 
    {
    }

    void TypeCheckVisitor::visit(ast::MemberExpression* node) 
    {
        auto func = node->object();
        assert(func->type() == ast::AstType::Identifier);

        ast::Identifier* id = dynamic_cast<ast::Identifier*>(func.get());
        auto psym = Environment::current()->get(id->token().sval);
        assert(!!psym);
        if (!psym) {

        }
    }

    void TypeCheckVisitor::visit(ast::CallArgs* node) 
    {
    }

    void TypeCheckVisitor::visit(ast::Identifier* node) 
    {
    }

    void TypeCheckVisitor::visit(ast::StringLiteral* node) 
    {
    }

};


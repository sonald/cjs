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

#include "ast.h"
#include "astvisitor.h"

namespace cjs
{
namespace ast
{
    SymbolPtr Literal::symbol()
    {
        return _env->get(_symName);
    }

    void Literal::annotate(string sym)
    {
        _symName = sym;
        _env = Environment::current();
    }

    void Program::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        bool first = true;
        for(auto& ptr: statements()) {
            if (!first)
                visitor->visit(AstVisitor::Phase::Step, this);
            ptr->visit(visitor);
            first = false;
        }
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void ExpressionStatement::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        expression()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void Expression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
    }

    void AssignExpression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        lhs()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Step, this);
        assignee()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void BinaryExpression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        lhs()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Step, this);
        rhs()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void UnaryExpression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        unary()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void PostfixExpression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        post()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void NewExpression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        member()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void CallExpression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        callee()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Step, this);
        args()->visit(visitor);
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void MemberExpression::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        object()->visit(visitor);
        if (property()) {
            visitor->visit(AstVisitor::Phase::Step, this);
            property()->visit(visitor);
        }
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void CallArgs::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Capture, this);
        bool first = true;
        auto& l = args();
        for_each(l.begin(), l.end(), [&](const ast::AstPtr& ptr) {
            if (!first)
                visitor->visit(AstVisitor::Phase::Step, this);
            ptr->visit(visitor);
            first = false;
        });
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    //leaf node, Capture and Bubble is merged into one Bubble
    void Identifier::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

    void Literal::visit(AstVisitor* visitor)
    {
        visitor->visit(AstVisitor::Phase::Bubble, this);
    }

};
};


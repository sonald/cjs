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

#include "astvisitor.h"
#include "ast.h"
#include "debug.h"

namespace cjs
{
    using namespace std;
    using namespace ast;
    DEF_DEBUG_FOR(Logger::Parser);
    DEF_ERROR_FOR(Logger::Parser);

    void ReprVisitor::visit(AstVisitor::Phase phase, ast::ExpressionStatement* node)
    {
        if (phase == AstVisitor::Phase::Bubble)
            _repr += ";\n";
    }

    void ReprVisitor::visit(Phase phase, ast::AssignExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            _repr += "(";
        } else if (phase == AstVisitor::Phase::Bubble) {
            _repr += ")";
        }
        if (phase == AstVisitor::Phase::Step) {
            _repr += " = ";
        }
    }

    void ReprVisitor::visit(Phase phase, ast::BinaryExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            _repr += "(";
        } else if (phase == AstVisitor::Phase::Bubble) {
            _repr += ")";
        }
        if (phase == AstVisitor::Phase::Step) {
            _repr += " " + node->op().sval + " ";
        }
    }

    void ReprVisitor::visit(Phase phase, ast::UnaryExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            _repr += "(";
            _repr += node->op().sval;
        } else if (phase == AstVisitor::Phase::Bubble) {
            _repr += ")";
        }
    }

    void ReprVisitor::visit(Phase phase, ast::PostfixExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            _repr += "(";
        } else if (phase == AstVisitor::Phase::Bubble) {
            _repr += node->op().sval;
            _repr += ")";
        }
    }

    void ReprVisitor::visit(Phase phase, ast::NewExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            _repr += "(new ";
        } else if (phase == AstVisitor::Phase::Bubble) {
            _repr += ")";
        }
    }

    void ReprVisitor::visit(AstVisitor::Phase phase, ast::CallExpression* node)
    {
    }

    void ReprVisitor::visit(AstVisitor::Phase phase, ast::MemberExpression* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            _repr += "(";
        } else if (phase == AstVisitor::Phase::Bubble) {
            _repr += ")";
        }
        if (phase == AstVisitor::Phase::Step) {
            if (node->property()) {
                _repr += ".";
            }
        }
    }

    void ReprVisitor::visit(AstVisitor::Phase phase, ast::CallArgs* node)
    {
        if (phase == AstVisitor::Phase::Capture) {
            _repr += "(";
        } else if (phase == AstVisitor::Phase::Step) {
            _repr += ",";
        } else if (phase == AstVisitor::Phase::Bubble) {
            _repr += ")";
        }
    }

    //FIXME: I dont know where does this node come from 
    //(e.g CallArgs or MemberExpression etc), may need parent node info
    void ReprVisitor::visit(AstVisitor::Phase phase, ast::Identifier* node)
    {
        _repr += node->token().sval;
    }

    void ReprVisitor::visit(AstVisitor::Phase phase, ast::Literal* node)
    {
        _repr += "\'" + node->token().sval + "\'";
    }

};


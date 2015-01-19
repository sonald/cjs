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

#ifndef _CJS_AST_H
#define _CJS_AST_H 

#include "lexer.h"
#include "env.h"

#include <memory>
#include <vector>

namespace cjs
{
    class AstVisitor;

namespace ast
{
    using namespace std;

    enum class AstType 
    {
        Program,
        StatementList,
        ExpressionStatement,
        AssignExpression,
        BinaryExpression,
        UnaryExpression,
        PostfixExpression,
        NewExpression,
        CallExpression,
        Identifier,
        StringLiteral,
        NullLiteral,
        BooleanLiteral,
        NumericLiteral,
        RegularExpressionLiteral,
        MemberExpression,
        CallArgs,
    };

    class Ast;
    using AstPtr = shared_ptr<Ast>;

    class Ast 
    {
        public:
            Ast(AstType ty): _type{ty} {}
            virtual void visit(AstVisitor* visitor) = 0;
            AstType type() const { return _type; }

        protected:
            AstType _type;
    };

    class Program: public Ast 
    {
        public:
            Program(): Ast(AstType::Program) {}
            virtual void visit(AstVisitor* visitor) override;
            vector<AstPtr>& statements() { return _statements; }
            template <class... Args>
            void append(Args... args) {
                _statements.emplace_back(args...);
            }

        protected:
            vector<AstPtr> _statements;
    };

    class ExpressionStatement: public Ast 
    {
        public:
            ExpressionStatement(Ast* expr)
                : Ast(AstType::ExpressionStatement), _expression{expr} {}
            virtual void visit(AstVisitor* visitor) override;
            AstPtr expression() { return _expression; }

        protected:
            AstPtr _expression;
    };

    class Expression: public Ast 
    {
        public:
            Expression(AstType ty): Ast(ty) {}
            virtual void visit(AstVisitor* visitor) override;
    };

    class AssignExpression: public Expression 
    {
        public:
            AssignExpression(Ast* lhs, Ast* assignee)
                : Expression(AstType::AssignExpression), _lhs{lhs}, _assignee{assignee} {}
            virtual void visit(AstVisitor* visitor) override;
            AstPtr lhs() { return _lhs; }
            AstPtr assignee() { return _assignee; }

        protected:
            AstPtr _lhs;  // lefthandside {NewExpression | CallExpression}
            AstPtr _assignee; // AssignExpression
    };

    class BinaryExpression: public Expression
    {
        public:
            BinaryExpression(Token op, Ast* lhs, Ast* rhs)
                : Expression(AstType::BinaryExpression), _op(op), _lhs{lhs}, _rhs{rhs}
            { }

            virtual void visit(AstVisitor* visitor) override;
            AstPtr lhs() { return _lhs; }
            AstPtr rhs() { return _rhs; }
            Token op() { return _op; }

        protected:
            Token _op;
            AstPtr _lhs, _rhs;
    };


    class UnaryExpression: public Expression
    {
        public:
            UnaryExpression(Token op, Ast* unary)
                : Expression(AstType::UnaryExpression), _op(op), _unary{unary}
            { }

            virtual void visit(AstVisitor* visitor) override;
            AstPtr unary() { return _unary; }
            Token op() { return _op; }

        protected:
            Token _op;
            AstPtr _unary;
    };

    class PostfixExpression: public Expression
    {
        public:
            PostfixExpression(Token op, Ast* post)
                : Expression(AstType::PostfixExpression), _op(op), _post{post}
            { }

            virtual void visit(AstVisitor* visitor) override;
            AstPtr post() { return _post; }
            Token op() { return _op; }

        protected:
            Token _op;
            AstPtr _post; // lefthandside
    };

    class NewExpression: public Expression
    {
        public:
            NewExpression(Ast* member)
                : Expression(AstType::NewExpression), _member{member}
            { }

            virtual void visit(AstVisitor* visitor) override;
            AstPtr member() { return _member; }

        protected:
            AstPtr _member;
    };

    class CallExpression: public Expression 
    {
        public:
            CallExpression(Ast* callee, Ast* args)
                : Expression(AstType::CallExpression), _callee{callee}, _args{args} {}
            virtual void visit(AstVisitor* visitor) override;
            AstPtr callee() { return _callee; }
            AstPtr args() { return _args; }

        protected:
            AstPtr _callee; // MemberExpr
            AstPtr _args; // CallArgs
    };

    class Identifier: public Expression 
    {
        public:
            Identifier(Token tk): Expression(AstType::Identifier), _token(tk) {}
            virtual void visit(AstVisitor* visitor) override;
            Token& token() { return _token; }

        protected:
            Token _token;
    };

    class Literal: public Expression 
    {
        public:
            Literal(Token tk, AstType ty): Expression(ty), _token(tk) {}
            virtual void visit(AstVisitor* visitor) override;
            Token& token() { return _token; }

            SymbolPtr symbol();
            void annotate(string sym);

        protected:
            Token _token;
            string _symName; // point to symbol table entry
            Environment* _env; // of this env
    };

    class MemberExpression: public Expression 
    {
        public:
            MemberExpression(Ast* obj, Ast* prop = nullptr)
                : Expression(AstType::MemberExpression), _object{obj}, _property{prop} {}
            virtual void visit(AstVisitor* visitor) override;
            AstPtr object() { return  _object; }
            AstPtr property() { return _property; }

        protected:
            AstPtr _object; //Primary or MemberExpr 
            AstPtr _property; //Primary could be null
    };

    class CallArgs: public Expression 
    {
        public:
            CallArgs(): Expression(AstType::CallArgs) {}
            virtual void visit(AstVisitor* visitor) override;
            vector<AstPtr>& args() { return _args; }
            template <class... Args>
            void append(Args... args) {
                _args.emplace_back(args...);
            }

        protected:
            vector<AstPtr> _args;
    };

}; // namespace ast
};

#endif

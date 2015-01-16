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

#ifndef _CJS_PARSER_H
#define _CJS_PARSER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "lexer.h"
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
                Bubble
            };

            virtual void visit(Phase phase, ast::Program* node) = 0;
            virtual void visit(Phase phase, ast::ExpressionStatement* node) = 0;
            virtual void visit(Phase phase, ast::Expression* node) = 0;
            virtual void visit(Phase phase, ast::CallExpression* node) = 0;
            virtual void visit(Phase phase, ast::MemberExpression* node) = 0;
            virtual void visit(Phase phase, ast::CallArgs* node) = 0;
            virtual void visit(Phase phase, ast::Identifier* node) = 0;
            virtual void visit(Phase phase, ast::StringLiteral* node) = 0;
    };

    class ReprVisitor: public AstVisitor
    {
        public:
            virtual void visit(Phase phase, ast::Program* node) override;
            virtual void visit(Phase phase, ast::ExpressionStatement* node) override;
            virtual void visit(Phase phase, ast::Expression* node) override;
            virtual void visit(Phase phase, ast::CallExpression* node) override;
            virtual void visit(Phase phase, ast::MemberExpression* node) override;
            virtual void visit(Phase phase, ast::CallArgs* node) override;
            virtual void visit(Phase phase, ast::Identifier* node) override;
            virtual void visit(Phase phase, ast::StringLiteral* node) override;
            string repr() const { return _repr; }

        private:
            string _repr;
    };

    class Parser
    {
        public:
            Parser();
            ast::Ast* program(const string& path);
            ast::Ast* expressionStatement();
            ast::Ast* expr();
            ast::Ast* callexpr();
            ast::Ast* memberexpr(ast::Ast* obj = nullptr);
            ast::Ast* args();
            ast::Ast* primary();
            ast::Ast* literal();
            ast::Ast* ident();

        private:
            unique_ptr<Tokenizer> _tokens; 
    };
};

#endif


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

#ifndef _CJS_STAGE_H
#define _CJS_STAGE_H 

#include <memory>
#include <vector>

#include "ast.h"

namespace cjs 
{
    using namespace std;
    class Backend;

    class Stage 
    {
        public:
            void setBackend(Backend* be);
            void start();
            void addCompilationUnit(const string& src);

        private:
            unique_ptr<Backend> _be;
            vector<ast::AstPtr> _ir;
    };

};

#endif

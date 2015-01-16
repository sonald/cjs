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

#include "backend.h"
#include "debug.h"

#include <cstdlib>
namespace cjs
{
    DEF_DEBUG_FOR(Logger::Backend);
    string MachBackend::compile(string src)
    {
        string cmd = "nasm -f macho64 " + src;
        debug("%", cmd);
        system(cmd.c_str());

        auto i = src.find_last_of('.');
        src.replace(src.begin()+i, src.end(), ".o");
        _objs.push_back(src);
        return src;
    }

    bool MachBackend::link()
    {
        string objs;
        for (auto& s: _objs) objs += s + " ";
        string cmd = "ld -demangle -dynamic -arch x86_64 -macosx_version_min 10.10.0 -o a.out " + objs + " -lSystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/6.0/lib/darwin/libclang_rt.osx.a";
        debug("%", cmd);
        system(cmd.c_str());
        return true;
    }
};

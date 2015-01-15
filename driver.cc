#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "nasm-emitter.h"

#include <string>
#include <iostream>

using namespace std;
using namespace cjs;

void compile(string src)
{
    string cmd = "nasm -f macho64 " + src;
    system(cmd.c_str());
}

void link(string obj)
{
    string cmd = "ld -demangle -dynamic -arch x86_64 -macosx_version_min 10.10.0 -o a.out " + obj + " -lSystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/6.0/lib/darwin/libclang_rt.osx.a";
    system(cmd.c_str());
}

int main(int argc, char *argv[])
{
    Parser parser;
    auto* ast = parser.program(argv[1]);

    ReprVisitor repr;
    ast->visit(&repr);
    cout << "ast: " << repr.repr() << endl;

    // build symtab and type-check
    TypeCheckVisitor tc;
    ast->visit(&tc);

    // emit nasm
    {
        // post runs on destructor
        ofstream os{"test.s"};
        NasmEmitterVisitor emitter {os};
        ast->visit(&emitter);
    }

    compile("test.s");
    link("test.o");
    return 0;
}

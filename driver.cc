#include "stage.h"
#include "backend.h"

#include <string>
#include <iostream>

#include <libgen.h>

using namespace std;
using namespace cjs;

string progname;
void usage()
{
    cout << R"(
cjs -o output filename
)";
    exit(0);
}

int main(int argc, char *argv[])
{
    progname = {basename(argv[0])};
    if (argc < 2) {
        usage();
    }

    Stage sg;
    sg.setBackend(new MachBackend);

    sg.addCompilationUnit(argv[1]);
    //sg.start();

    return 0;
}

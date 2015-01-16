#include "stage.h"
#include "backend.h"

#include <string>
#include <iostream>

using namespace std;
using namespace cjs;

int main(int argc, char *argv[])
{
    Stage sg;
    sg.setBackend(new MachBackend);

    sg.addCompilationUnit(argv[1]);
    sg.start();

    return 0;
}

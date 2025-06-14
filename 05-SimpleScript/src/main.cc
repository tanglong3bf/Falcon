#include "repl.hpp"

int main(int argc, char* argv[])
{
    Repl repl;
    if (argc == 2 &&
        (strcmp(argv[1], "--verbose") == 0 || strcmp(argv[1], "-v") == 0))
    {
        repl.setVerbose(true);
    }
    repl.run();
    return 0;
}

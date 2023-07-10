#include "assembler/assembler.h"
#include "global/defines.h"

#define DEBUG 1
#define CHECK_MEMORY_LEAKS 1

int main(int argc, char  **argv)
{
    return assemble(argc, argv);
}
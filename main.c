#include "assembler/assembler.h"
#include "global/defines.h"

#define DEBUG
#define CHECK_MEMORY_LEAKS

int main(int argc, char  **argv)
{
    if(assemble(argc, argv) == FAILURE){
        return 1;
    }
    return 0;
}
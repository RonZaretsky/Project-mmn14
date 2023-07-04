#include "assembler.h"
#include "defines.h"

int main(int argc, char const *argv[])
{
    if(assemble(argc, argv) == FAILURE){
        return 1;
    }
    return 0;
}
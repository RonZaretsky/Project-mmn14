#include "assembler.h"
#include "../preprocessor/preprocessor.h"
#include <stdio.h>
#include "../global/defines.h"


int assemble(int file_count, char **file_names){
    int i;
    for(i = 1; i < file_count; i++){
        if(preprocesses_file(file_names[i]) == FAILURE){
            fprintf(stderr, "%sError%s: Failed to preprocess file '%s%s%s'\n", BRED, reset, AS_FILES_PATH,file_names[i], AS_FILE_EXTENSION);
        }
    }
    return SUCCESS;
}


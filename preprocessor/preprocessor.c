#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "preprocessor.h"

void * preprocesses_file(const char *filename){
    FILE *file = open_file(filename);
    if(file == NULL) return NULL;
}

static FILE * open_file(const char *filename){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        fprintf(stderr, "Error: could not open file %s\n", filename);
        return NULL;
    }
    return file;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include "../global/defines.h"
#include "../global/utils.c"
#include "../global/dir_ins_names.h"
#include "preprocessor.h"

static void * macro_ctor(void const * macro);
static void macro_dtor(void * macro);
static void * am_line_ctor(void const * line);
static void am_line_dtor(void * line);
static void load_content_to_am_file(FILE *am_file_ptr, Vector *file_content);
static int open_file(FILE **file_ptr, const char *path, const char *file_name, const char *extension, const char * mode);

typedef struct file_line{
    char line_content[MAX_LINE_LENGTH];
} FileLine;

int preprocesses_file(const char *file_name){
    Trie macros;
    FILE *as_file_ptr;
    FILE *am_file_ptr;
    Vector macros_content;
    Vector file_content;

    /* open as file for reasing */
    if(open_file(&as_file_ptr, AS_FILES_PATH, file_name, AS_FILE_EXTENSION, "r") == FAILURE){
        return FAILURE;
    }


    


    /* open am file for writing */
    if(open_file(&am_file_ptr, AM_FILES_PATH, file_name, AM_FILE_EXTENSION, "w") == FAILURE){
        fclose(as_file_ptr);
        return FAILURE;
    }
    
    return SUCCESS;
}


static void * macro_ctor(void const * macro){
    char * macro_content = (char *)malloc(strlen(macro) + 1);
    if(macro_content == NULL){
        fprintf(stderr, "Error: could not allocate memory for macro\n");
        return NULL;
    }
    strcpy(macro_content, macro);
    return macro_content;
}

static void macro_dtor(void * macro){
    free(macro);
}

static void * am_line_ctor(void const * copy){
    FileLine * file_line = malloc(sizeof(FileLine));
    if(file_line == NULL){
        fprintf(stderr, "Error: could not allocate memory for file_line\n");
        return NULL;
    }
    return memcpy(file_line, copy, sizeof(FileLine));
}

static void am_line_dtor(void * line){
    free(line);
}

static void load_content_to_am_file(FILE *am_file_ptr, Vector *file_content){
    void * const * begin;
    void * const * end;
    VECTOR_FOR_EACH(begin,end, *file_content){
        fprintf(am_file_ptr, "%s", ((FileLine *)*begin)->line_content);
    }
}

static int open_file(FILE **file_ptr, const char *path, const char *file_name, const char *extension, const char *mode){
    char full_path[MAX_PATH_LENGTH] = {0};
    strcpy(full_path, path);
    strcat(full_path, file_name);
    strcat(full_path, extension);
    *file_ptr = fopen(full_path, mode);
    if(*file_ptr == NULL){
        fprintf(stderr, "%sError%s: could not open file '%s'\n", BRED,reset, full_path);
        return(FAILURE);
    }
    return SUCCESS;
}

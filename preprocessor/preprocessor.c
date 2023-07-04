#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data_structures/vector/vector.h"
#include "data_structures/trie/trie.h"
#include "defines.h"
#include "preprocessor.h"

static FILE * open_file(const char *file_name);
static void * macro_ctor(void const * macro);
static void macro_dtor(void * macro);
static void close(FILE **as_file_ptrr, FILE **am_file_ptrr, Trie *macros, Vector *macros_content);


int preprocesses_file(const char *file_name){
    FILE *as_file_ptr = open_file(file_name);
    FILE *am_file_ptr;
    Trie macros = trie();
    Vector macros_content = vector_ctor(macro_ctor, macro_dtor);
    if(macros_content == NULL || as_file_ptr == NULL){
        close(&as_file_ptr, &am_file_ptr, &macros, &macros_content);
        return FAILURE;
    }
    
    
    close(&as_file_ptr, &am_file_ptr, &macros, &macros_content);
    return SUCCESS;
}

static FILE * open_file(const char *file_name){
    char * file_name_with_ext = strcat(file_name, AM_FILE_EXTENSION);
    FILE *file = fopen(file_name_with_ext, "r");
    if(file == NULL){
        fprintf(stderr, "Error: could not open file %s\n", file_name_with_ext);
        return NULL;
    }
    return file;
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


static void close(FILE **as_file_ptrr, FILE **am_file_ptrr, Trie *macros, Vector *macros_content)
{
    trie_destroy(macros);
    vector_destroy(macros_content);
    fclose(*as_file_ptrr);
    fclose(*am_file_ptrr);
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include "../global/defines.h"
#include "preprocessor.h"

static void * macro_ctor(void const * macro);
static void macro_dtor(void * macro);
static void close(FILE **as_file_ptrr, FILE **am_file_ptrr, Trie *macros, Vector *macros_content);


int preprocesses_file(const char *file_name){
    Trie macros;
    FILE *as_file_ptr;
    FILE *am_file_ptr;
    Vector macros_content;
    char full_path[MAX_PATH_LENGTH];

    /* open as file for reasing */
    strcpy(full_path, AS_FILES_PATH);
    strcat(full_path, file_name);
    strcat(full_path, AS_FILE_EXTENSION);
    if((as_file_ptr = fopen(full_path, "r")) == NULL){
        fprintf(stderr, "Error: could not open file '%s'\n", full_path);
        return FAILURE;
    }

    /* Reset the full_path string */ 
    memset(full_path, 0, sizeof(full_path));

    /* open am file for appending */
    strcpy(full_path, AM_FILES_PATH);
    strcat(full_path, file_name);
    strcat(full_path, AM_FILE_EXTENSION);
    am_file_ptr = fopen(full_path, "a");
    if (am_file_ptr == NULL)
    {
        fprintf(stderr, "Error: could not open file '%s'\n", full_path);
        fclose(as_file_ptr);
        return FAILURE;
    }
    

    /* decalre Trie and Vector */
    macros = trie();
    macros_content = new_vector(macro_ctor, macro_dtor);
    if(macros_content == NULL){
        fprintf(stderr, "Error: could not allocate memory for macros_content\n");
        close(&as_file_ptr, &am_file_ptr, &macros, &macros_content);
        return FAILURE;
    }
    
    
    close(&as_file_ptr, &am_file_ptr, &macros, &macros_content);
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

static void close(FILE **as_file_ptrr, FILE **am_file_ptrr, Trie *macros, Vector *macros_content)
{
    trie_destroy(macros);
    vector_destroy(macros_content);
    if(*as_file_ptrr) fclose(*as_file_ptrr);
    if(*am_file_ptrr) fclose(*am_file_ptrr);
}

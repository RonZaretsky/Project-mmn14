#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include "../global/defines.h"
#include "../global/dir_ins_names.h"
#include "preprocessor.h"

#define SKIP_SPACES(line) while(*line != ' ' && *line != '\t') line++
#define SKIP_MCRO(line) while(*line == 'm' || *line == 'c' || *line == 'r' || *line == 'o') line++

typedef enum LineType{
    START_MACRO_DEF,
    END_MACRO_DEF,
    MACRO_CALL,
    NORMAL_LINE,
    COMMENT_OR_EMPTY_LINE
} LineType ;

typedef struct Macro {
    char name[MAX_LINE_LENGTH];
    Vector lines;
} Macro;

static int open_file(FILE **file_ptr, const char *path, const char *file_name, const char *extension, const char * mode);
static void* macro_ctor(const void * copy);
static void macro_dtor(void * item);
static void* line_ctor(const void * copy);
static void line_dtor(void * item);
static void load_am_file(FILE ** file, Vector *file_content);
static int preprocess_macros_line_by_line(FILE **file, Vector *new_file_content);
static LineType get_line_type(char *line, Macro **macro);






const char * preprocesses_file(const char *file_name){
    FILE *as_file_ptr;
    FILE *am_file_ptr;
    Vector file_content;
    file_content = new_vector(line_ctor, line_dtor);

    /* open as file for reasing */
    if(open_file(&as_file_ptr, AS_FILES_PATH, file_name, AS_FILE_EXTENSION, "r") == FAILURE){
        vector_destroy(&file_content);
        return FAILURE;
    }

    

    preprocess_macros_line_by_line(&as_file_ptr, &file_content);
    


    /* open am file for writing */
    if(open_file(&am_file_ptr, AM_FILES_PATH, file_name, AM_FILE_EXTENSION, "w") == FAILURE){
        vector_destroy(&file_content);
        fclose(as_file_ptr);
        return FAILURE;
    }

    load_am_file(&am_file_ptr, &file_content);

    
    vector_destroy(&file_content);
    fclose(as_file_ptr);
    fclose(am_file_ptr);
    return SUCCESS;
}



static int preprocess_macros_line_by_line(FILE **file, Vector *new_file_content){
    char line[MAX_LINE_LENGTH];
    Macro *macro;
    LineType line_type;
    Trie macros = trie();
    Vector macros_content = new_vector(macro_ctor, macro_dtor); 



    while(fgets(line, sizeof(line), *file) != NULL){
        line_type = get_line_type(line, &macro);
        switch(line_type){
            case START_MACRO_DEF:
                break;
            case END_MACRO_DEF:
                break;
            case MACRO_CALL:
                break;
            case NORMAL_LINE:
                vector_insert(*new_file_content, line);
                break;
            case COMMENT_OR_EMPTY_LINE:
                break;
        }
    }

    trie_destroy(&macros);
    vector_destroy(&macros_content);
    return SUCCESS;
}

static LineType get_line_type(char *line, Macro **macro){
    return NORMAL_LINE;
}

























static void load_am_file(FILE ** file, Vector *file_content){
    void * const * begin;
    void * const * end;
    char line[MAX_LINE_LENGTH] = {0};
    
    VECTOR_FOR_EACH(begin,end,(*file_content)){
        if(*begin != NULL) fprintf(*file, "%s", (char*)*begin);
    }
}

static int open_file(FILE **file_ptr, const char *path, const char *file_name, const char *extension, const char *mode){
    char full_path[MAX_STRING_LENGTH] = {0};
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

static void* macro_ctor(const void * copy){
    Macro *macro = malloc(sizeof(Macro));
    strcpy(macro->name, ((Macro*)copy)->name);
    macro->lines = new_vector(line_ctor, line_dtor);
    return macro;
}

static void macro_dtor(void * item){
    Macro  *macro = item;
    vector_destroy(&macro->lines);
    free((void*)macro);
}

static void* line_ctor(const void * copy){
    return strcpy(malloc(strlen(copy) + 1), copy);
}

static void line_dtor(void * item){
    free(item);
}




















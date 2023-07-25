#include "preprocessor.h"
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../global/defines.h"
#include <ctype.h>
#include "../global/dir_ins_names.h"

#define SKIP_SPACES(line) while(*line && isspace(*line)) line++


typedef struct macro{
    char name[MAX_LINE_LENGTH+1];
    Vector lines;
} Macro;

enum line_type{
    START_MACRO,
    END_MACRO,
    CALL_MACRO,
    COMMENT_OR_EMPTY,
    OTHER,
};


static enum line_type get_line_type(const char * line, Trie * macro_names, Vector * macro_table ,Macro ** macro);
static void * macro_ctor(const void * copy);
static void macro_dtor(void * item);
static void * line_ctor(const void * copy);
static void line_dtor(void * item);
static int is_mcro_name_dir_or_op(const char * line);
static void load_am_file(FILE ** file, Vector * file_content);



const char * preprocesses_file(const char *file_name){
    char line[MAX_LINE_LENGTH+1] = {0};
    char * macro_name;

    char as_file_name[MAX_STRING_LENGTH+1] = {0};
    char am_file_name[MAX_STRING_LENGTH+1] = {0};

    Vector am_file_lines;
    Vector macro_table;
    Trie macro_names;

    FILE * as_file;
    FILE * am_file;

    Macro * macro;

    int is_macro_def = FALSE;
    int is_error = FALSE;

    void * const * begin;
    void * const * end;


    strcpy(as_file_name,AS_FILES_PATH);
    strcat(as_file_name,file_name);
    strcat(as_file_name,AS_FILE_EXTENSION);
    strcpy(am_file_name,AM_FILES_PATH);
    strcat(am_file_name,file_name);
    strcat(am_file_name,AM_FILE_EXTENSION);

    as_file = fopen(as_file_name, "r");

    if(!as_file){
        printf("Error: file %s does not exist.\n", as_file_name);
        return NULL;
    }

    macro_table = new_vector(macro_ctor, macro_dtor);
    am_file_lines = new_vector(line_ctor, line_dtor); 
    macro_names = trie();
    
    while(fgets(line, sizeof(line), as_file) && !is_error){
        switch(get_line_type(line, &macro_names, &macro_table, &macro)){
            case START_MACRO:
                break;
            case END_MACRO: 
                break;
            case CALL_MACRO: 
                break;
            case COMMENT_OR_EMPTY:
                break;
            case OTHER: 
                if(is_macro_def){

                }else{
                    vector_insert(am_file_lines, line);
                }
                break;
            default: break;
        }
    }
    
    if(!is_error){
        am_file = fopen(am_file_name, "w");
        load_am_file(&am_file, &am_file_lines);
        fclose(am_file);
    }
    
    vector_destroy(&macro_table);
    vector_destroy(&am_file_lines);
    trie_destroy(&macro_names);
    fclose(as_file);
}

static enum line_type get_line_type(const char * line, Trie * macro_names, Vector * macro_table ,Macro ** macro){
    char *word;
    char line_copy[MAX_LINE_LENGTH+1] = {0};
    strcpy(line_copy, line);
    char * line_ptr = line_copy;
    word = strchr(line_ptr, ';');
    if(word) *word = '\0';
    SKIP_SPACES(line_ptr);
    if(*line_ptr == '\0') return COMMENT_OR_EMPTY;

    return OTHER;
}

static void * macro_ctor(const void * copy){
    Macro * macro = copy;
    Macro * new_macro = malloc(sizeof(Macro));
    strcpy(new_macro->name, macro->name);
    new_macro->lines = new_vector(line_ctor, line_dtor);
    return new_macro;
}

static void macro_dtor(void * item){
    Macro * macro = item;
    vector_destroy(&macro->lines);
    free(macro);
}

static void * line_ctor(const void * copy){
    return strcpy(malloc(strlen(copy) + 1), (char*)copy);
}

static void line_dtor(void * item){
    free(item);
}

static void load_am_file(FILE ** file, Vector * file_content){
    void * const * begin;
    void * const * end;
    char line[MAX_LINE_LENGTH+1] = {0};

    VECTOR_FOR_EACH(begin, end, *file_content){
        if(*begin != NULL) fprintf(*file, "%s", (char*)*begin);
    }
}

static int is_mcro_name_dir_or_op(const char * line){
    int i;
    for( i = 0; i < DIRECTIVES_COUNT; i++){
        if(!strcmp(line, directives[i])){
            /* mcro name cant be a directive name*/
            fprintf(stderr, "Error: macro name can't be a directive name.\n");
            return FAILURE;
        }
    }
    for( i = 0; i < INSTRUCTIONS_COUNT; i++){
        if(!strcmp(line, instructions[i])){
            /* mcro name cant be a instuction name*/
            fprintf(stderr, "Error: macro name can't be a instruction name.\n");
            return FAILURE;
        }
    }
    return SUCCESS;
}

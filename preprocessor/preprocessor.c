#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "preprocessor.h"
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include "../global/defines.h"
#include "../global/dir_ins_names.h"


/* macro struct */
typedef struct macro{
    char name[MAX_LINE_LENGTH+1];
    Vector lines;
} Macro;

/* line type enum */
enum line_type{
    START_MACRO,
    END_MACRO,
    CALL_MACRO,
    COMMENT_OR_EMPTY,
    OTHER,
    BAD_ENDMCRO_CALL,
    BAD_NEW_MACRO_CALL,
    MACRO_ALREADY_EXISTS
};

/* Signatures of methods */
static enum line_type get_line_type(const char * line, Trie * macro_names, Vector * macro_table ,Macro ** macro);
static void * macro_ctor(const void * copy);
static void macro_dtor(void * item);
static void * line_ctor(const void * copy);
static void line_dtor(void * item);
static int is_mcro_name_dir_or_op(const char * line);
static void load_am_file(FILE ** file, Vector * file_content);



const int preprocesses_file(const char *file_name){
    char line[MAX_LINE_LENGTH+1] = {0};
    char * macro_name;

    char as_file_name[MAX_STRING_LENGTH+1] = {0};
    char am_file_name[MAX_STRING_LENGTH+1] = {0};

    Vector am_file_lines = NULL;
    Vector macro_table = NULL;
    Trie macro_names = NULL;

    FILE * as_file;
    FILE * am_file;

    Macro * macro;
    Macro * s_macro;

    int is_macro_def = FALSE;
    int is_error = FALSE;

    void * const * begin;
    void * const * end;

    /*get am and am files including the full path*/
    strcpy(as_file_name,AS_FILES_PATH);
    strcat(as_file_name,file_name);
    strcat(as_file_name,AS_FILE_EXTENSION);
    strcpy(am_file_name,AM_FILES_PATH);
    strcat(am_file_name,file_name);
    strcat(am_file_name,AM_FILE_EXTENSION);

    /* open as file */
    as_file = fopen(as_file_name, "r");

    /* error if file does not exist */
    if(!as_file){
        printf("Error: file %s does not exist.\n", as_file_name);
        return FAILURE;
    }

    macro_table = new_vector(macro_ctor, macro_dtor);
    am_file_lines = new_vector(line_ctor, line_dtor); 
    macro_names = trie();
    

    /* read as file line by line */
    while(fgets(line, sizeof(line), as_file) && !is_error){
        /* check line type */
        switch(get_line_type(line, &macro_names, &macro_table, &macro)){
            case START_MACRO:
                is_macro_def = TRUE;
                break;
            case END_MACRO: 
                free(macro);
                is_macro_def = FALSE;
                break;
            case CALL_MACRO:
                /* insert macro lines to am file if a macro called */
                macro_name = strtok(line, SPACE_CHARS);
                s_macro = trie_exists(macro_names, macro_name);
                VECTOR_FOR_EACH(begin, end, s_macro->lines){
                    if(*begin)
                        vector_insert(am_file_lines, *begin);
                }
                break;
            case COMMENT_OR_EMPTY:
                break;
            case BAD_NEW_MACRO_CALL:
            case MACRO_ALREADY_EXISTS:
            case BAD_ENDMCRO_CALL:
                /* error cases */
                is_error = TRUE;
                break;
            case OTHER:
                /* inserts line to am file or to macri ,depends on if it a macro def*/
                if(is_macro_def){
                    vector_insert(((Macro*)(trie_exists(macro_names, macro->name)))->lines, line);
                }else{
                    vector_insert(am_file_lines, line);
                }
                break;
            default: break;
        }
    }
    
    /* load am file if no errors */
    if(!is_error){
        am_file = fopen(am_file_name, "w");
        load_am_file(&am_file, &am_file_lines);
        fclose(am_file);
    }
    

    /* free al memory */
    vector_destroy(&macro_table);
    vector_destroy(&am_file_lines);
    trie_destroy(&macro_names);
    fclose(as_file);
    return SUCCESS;
}

/* this method return the type of the line*/
static enum line_type get_line_type(const char * line, Trie * macro_names, Vector * macro_table ,Macro ** macro){
    char * line_ptr;
    char *word;
    char line_copy[MAX_LINE_LENGTH+1] = {0};
    strcpy(line_copy, line);
    line_ptr = line_copy;
    word = strchr(line_ptr, ';');
    if(word) *word = '\0';
    SKIP_SPACES(line_ptr);
    if(*line_ptr == '\0') return COMMENT_OR_EMPTY;
    word = strtok(line_ptr, SPACE_CHARS);
    if(!strcmp(word, MCRO)) {
        /*if firest word is mcro so it is start of mcro def */
        line_ptr += strlen(word)+1;
        word = strtok(line_ptr, SPACE_CHARS);
        if(!is_mcro_name_dir_or_op(word)){ 
            return BAD_NEW_MACRO_CALL;
        }
        /* error if mcro already exists */
        if(trie_exists(*macro_names, word)) {
            fprintf(stderr, "Error: macro %s already exists.\n", word);
            return MACRO_ALREADY_EXISTS;
        }
        line_ptr += strlen(word)+1;
        SKIP_SPACES(line_ptr);
        /* error if extra characters after new macro call */
        if(*line_ptr != '\0') {
            fprintf(stderr, "Error: extra characters after new macro call '%s'.\n", word);
            return BAD_NEW_MACRO_CALL;
        }
        *macro = malloc(sizeof(Macro));
        strcpy((*macro)->name, word);
        trie_insert(*macro_names, word, vector_insert(*macro_table, *macro));
        return START_MACRO;
    } else if(!strcmp(word, ENDMCRO)) {
        /* if first word is endmcro so it is end of mcro def*/
        line_ptr += strlen(word)+1;
        SKIP_SPACES(line_ptr);
        /* error if extra characters after endmcro */
        if(*line_ptr != '\0') {
            free(*macro);
            fprintf(stderr, "Error: extra characters after endmcro.\n");
            return BAD_ENDMCRO_CALL;
        }
        
        return END_MACRO;
    } else if(trie_exists(*macro_names,word)){
        /* if first word is macro name so it is call of macro*/
        return CALL_MACRO;
    }
    return OTHER;
}

/* macro vector constructor */
static void * macro_ctor(const void * copy){
    Macro * macro = (Macro*)copy;
    Macro * new_macro = malloc(sizeof(Macro));
    strcpy(new_macro->name, macro->name);
    new_macro->lines = new_vector(line_ctor, line_dtor);
    return new_macro;
}
/* macro vector distructor*/
static void macro_dtor(void * item){
    Macro * macro = item;
    vector_destroy(&macro->lines);
    free(macro);
}
/* lines vector constructor*/
static void * line_ctor(const void * copy){
    return strcpy(malloc(strlen(copy) + 1), (char*)copy);
}
/* lines vector distructor*/
static void line_dtor(void * item){
    free(item);
}
/* this method loads am file with lines from vector*/
static void load_am_file(FILE ** file, Vector * file_content){
    void * const * begin;
    void * const * end;

    VECTOR_FOR_EACH(begin, end, *file_content){
        if(*begin != NULL) fprintf(*file, "%s", (char*)*begin);
    }
}
/* this method checks if macro's names id direction or operation*/
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

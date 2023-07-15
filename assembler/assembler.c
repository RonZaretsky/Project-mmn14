#include "assembler.h"
#include "../lexer/lexer.h"
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include <stdio.h>
#include "../global/defines.h"
#include <string.h>
#include <stdlib.h>

/* a symbol struct */
typedef struct symbol{
    enum{
        symbol_extern,
        symbol_entry,
        symbol_code,
        symbol_data,
        symbol_entry_code,
        symbol_entry_data
    } type;
    unsigned int address;
    char name[MAX_SYMBOL_LENGTH+1];
} symbol;

typedef struct missing_symbol{
    char name[MAX_SYMBOL_LENGTH];
    unsigned int * address;
} missing_symbol;

/* an object file struct */
typedef struct object_file{
    Vector code_image;
    Vector data_image;
    Vector symbol_table;
    Vector extern_symbols_table;
    Vector missing_symbols_table;
    Trie symbols_names;
} object_file;

typedef struct extern_call{
    char extern_name[MAX_SYMBOL_LENGTH + 1];
    Vector call_address;
} extern_call;

/* Signatures of methods */
static int compile(FILE * file, object_file * objfile);
static void * ctor_mem_word(const void * copy);
static void dtor_mem_word(void * to_delete);
static void * ctor_symbol(const void * copy);
static void dtor_symbol(void * to_delete);
static void * extern_call_ctor(const void * copy);
static void extern_call_dtor(void * to_delete);
static void * missing_symbol_ctor(const void * copy);
static void missing_symbol_dtor(void * to_delete);
static void add_extern(Vector extern_calls, const char * extern_name, const unsigned int address);

/* Main function of assembler*/
int assemble(int file_count, char **file_names){
    return 0;
}


/* Implemention of methods */

/* compile method */
static int compile(FILE * file, object_file * objfile){
    missing_symbol m_symbol;
    char line[MAX_LINE_LENGTH] = {0};
    unsigned int line_cnt = 0;
    assembler_ast ast = {0};
    int has_error = FALSE;
    extern_call e_call;
    symbol current_symbol;
    symbol *exists_symbol;
    unsigned int extern_call_adress = 0;
    unsigned int machine_word = 0;
    int i;
    unsigned int * just_inserted;

    /*  runs through all the lines in am file after the preprocess
        each row goes through a logical checking process */
    while(fgets(line, MAX_LINE_LENGTH, file)){
        /* converts every line to ast */
        ast = line_to_ast_lexer(line);

        /* check if the line isn't empty and not syntac error - so it definetrly a symbol*/
        if(ast.label_name[0] != END_OF_STRING && ast.line_type != syntax_error){
            /* copies the symbol name */
            strcpy(current_symbol.name, ast.label_name);
            exists_symbol = trie_exists(objfile->symbols_names, ast.label_name);

            /* if symbol is operation */
            if(ast.line_type == op){
                if(exists_symbol){
                    if(exists_symbol->type != symbol_entry){
                        /* ERROR REDEF */
                    }
                    else {
                        exists_symbol->type = symbol_entry_code;
                        exists_symbol->address = vector_get_items_count(objfile->code_image) + 100;
                    }
                }
                else {
                    current_symbol.type = symbol_code;
                    current_symbol.address = vector_get_items_count(objfile->code_image) + 100;
                    trie_insert(objfile->symbols_names, current_symbol.name, vector_insert(objfile->symbol_table, &current_symbol));
                }
            } 
            /* if symbol is directive */
            else {
                if(exists_symbol){
                    if(ast.op_or_dir.dir_line.dir_type >= dir_string){
                        if(exists_symbol->type != symbol_entry ){
                            /* ERROR REDEF */
                        }
                        else {
                            exists_symbol->type = symbol_entry_data;
                        }
                    }
                }
                else {
                    if(ast.op_or_dir.dir_line.dir_type >= dir_string){
                        current_symbol.type = symbol_entry_data;
                        current_symbol.address = vector_get_items_count(objfile->data_image);
                        trie_insert(objfile->symbols_names, current_symbol.name, vector_insert(objfile->symbol_table, &current_symbol));
                    }

                }

            }
        }
        /* if there is a syntax error */
        if(ast.line_type == syntax_error){
            printf(RED "Syntax error: %s\n" reset, ast.error_msg);
            has_error = TRUE;
        /* if the line is directive */
        } else if (ast.line_type == dir){
            if(ast.op_or_dir.dir_line.dir_type == dir_data){

            }
        /* if the line is operation */
        } else if (ast.line_type == op){
            /* insertion of opcode machine word to code image*/
            machine_word = 0;
            machine_word |= ast.op_or_dir.op_line.op_operand_option[1] << 2;
            machine_word |= ast.op_or_dir.op_line.op_operand_option[0] << 9;
            machine_word |= ast.op_or_dir.op_line.op_type << 5;
            vector_insert(objfile->code_image, &machine_word);
            if (ast.op_or_dir.op_line.op_type >= op_mov && ast.op_or_dir.op_line.op_type <= op_jsr){
                if(ast.op_or_dir.op_line.op_operand_option[1] == reg && ast.op_or_dir.op_line.op_operand_option[0] == reg){
                    /* if both operands are register so it optimized to one machine word */
                    machine_word = 0;
                    machine_word |= ast.op_or_dir.op_line.op_content[1].reg_num << 2;  
                    machine_word |= ast.op_or_dir.op_line.op_content[0].reg_num << 7;  
                    vector_insert(objfile->code_image, &machine_word);
                } else {
                    /* otherwise two machine words */
                    for(i = 0; i< 2; i++){
                        switch(ast.op_or_dir.op_line.op_operand_option[i]){
                            /* if const number */
                            case number:
                                machine_word = ast.op_or_dir.op_line.op_content[i].const_num << 2;
                                vector_insert(objfile->code_image, &machine_word);
                                break;
                            /* if register */
                            case reg:
                                if(i == 0)
                                    machine_word = ast.op_or_dir.op_line.op_content[i].reg_num << 7;
                                else
                                    machine_word = ast.op_or_dir.op_line.op_content[i].reg_num << 2;
                                vector_insert(objfile->code_image, &machine_word);
                                break;
                            /* if label */
                            case label:
                                exists_symbol = trie_exists(objfile->symbols_names, ast.op_or_dir.op_line.op_content[i].label_name);
                                if(exists_symbol && exists_symbol->type != symbol_entry){
                                    machine_word = exists_symbol->address << 2;
                                    if(exists_symbol->type == symbol_extern ){
                                        machine_word |= 1;
                                        extern_call_adress = vector_get_items_count(objfile->code_image) + 100 + 1;
                                        add_extern(objfile->extern_symbols_table, exists_symbol->name, extern_call_adress);
                                        strcpy(m_symbol.name, exists_symbol->name);
                                        vector_insert(e_call.call_address, &extern_call_adress);
                                    } else {
                                        machine_word |= 2;
                                    }
                                }
                                just_inserted = vector_insert(objfile->code_image, &machine_word);
                                if(!exists_symbol || (exists_symbol && exists_symbol->type == symbol_entry)){
                                    /* incase no symbol found, save the pointer to update the adress later */
                                    strcpy(m_symbol.name, ast.op_or_dir.op_line.op_content[i].label_name);
                                    m_symbol.address = just_inserted;
                                    vector_insert(objfile->missing_symbols_table, &m_symbol);
                                }
                                break;
                            case none:
                                break;
                        }
                        
                    }

                }

            } else if (ast.op_or_dir.op_line.op_type >= op_not && ast.op_or_dir.op_line.op_type <= op_jsr){
                

            } else {
                /* this is for rst and stop op codes but it does nothing so it is empty */
            }
        }
        line_cnt++;
    }
    
    
    return has_error;
}

/* this is a constructor for machine word vectors*/
static void * ctor_mem_word(const void * copy){
    return memcpy(malloc(sizeof(unsigned int)), copy, sizeof(unsigned int));
}
/* this is a distructor for machine word vectors*/
static void dtor_mem_word(void * to_delete){
    free(to_delete);
}
/* this is a constructor for symbol vectors*/
static void * ctor_symbol(const void * copy){
    return memcpy(malloc(sizeof(symbol)), copy, sizeof(symbol));;
}
/* this is a distructor for symbol vectors*/ 
static void dtor_symbol(void * to_delete){
    free(to_delete);
}

static void * extern_call_ctor(const void * copy){
    return memcpy(malloc(sizeof(extern_call)), copy, sizeof(extern_call));

}

static void extern_call_dtor(void * to_delete){
    extern_call * e_call = to_delete;
    vector_destroy(&(e_call->call_address));
    free(e_call);

}

static void * ctor_missing_symbol(const void * copy){
    return memcpy(malloc(sizeof(missing_symbol)), copy, sizeof(missing_symbol));
}

static void dtor_missing_symbol(void * to_delete){
    free(to_delete);
}

static void add_extern(Vector extern_calls, const char * extern_name, const unsigned int address){
    void * const * begin;
    void * const * end;
    extern_call e_call = {0};
    VECTOR_FOR_EACH(begin,end,extern_calls){
        if(*begin){
            if(strcmp(extern_name, ((const extern_call *)(*begin))->extern_name) == 0){
                vector_insert(((extern_call *)(*begin))->call_address, &address);
                return;
            }
        }
    }
    strcpy(e_call.extern_name, extern_name);
    e_call.call_address = new_vector(ctor_mem_word, dtor_mem_word);
    vector_insert(e_call.call_address, &address);
    vector_insert(extern_calls, &e_call);
}
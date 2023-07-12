#include "assembler.h"
#include "../lexer/lexer.h"
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include <stdio.h>
#include "../global/defines.h"
#include <string.h>

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
    char * name;
} symbol;

typedef struct missing_symbols{
    char name[MAX_SYMBOL_LENGTH];
    unsigned int * address;
} missing_symbols;

/* an object file struct */
typedef struct object_file{
    Vector code_image;
    Vector data_image;
    Vector symbol_table;
    Vector missing_symbols_table;
    Trie symbols_names;
} object_file;

/* Signatures of methods */
static int compile(FILE * file, object_file * objfile);

/* Main function of assembler*/
int assemble(int file_count, char **file_names){
    return 0;
}


/* Implemention of methods */

/* compile method */
static int compile(FILE * file, object_file * objfile){
    missing_symbols m_symbol;
    char line[MAX_LINE_LENGTH] = {0};
    unsigned int line_cnt = 0;
    assembler_ast ast;
    int has_error = FALSE;
    symbol current_symbol;
    symbol *exists_symbol;
    unsigned int IC = 100;
    unsigned int DC = 0;
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
                        exists_symbol->address = IC;
                        IC++;
                    }
                }
                else {
                    current_symbol.type = symbol_code;
                    current_symbol.address = IC;
                    IC++;
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
                        current_symbol.address = DC;
                        DC++;
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
            machine_word = 0;
            machine_word |= ast.op_or_dir.op_line.op_operand_option[1] << 2;
            machine_word |= ast.op_or_dir.op_line.op_operand_option[0] << 9;
            machine_word |= ast.op_or_dir.op_line.op_type << 5;
            vector_insert(objfile->code_image, &machine_word);
            if ((ast.op_or_dir.op_line.op_type >= op_mov && ast.op_or_dir.op_line.op_type <= op_sub )|| (ast.op_or_dir.op_line.op_type == op_lea)){
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
                            case number:
                                machine_word = ast.op_or_dir.op_line.op_content[i].const_num << 2;
                                break;
                            case reg:
                                if(i == 0)
                                    machine_word = ast.op_or_dir.op_line.op_content[i].reg_num << 7;
                                else
                                    machine_word = ast.op_or_dir.op_line.op_content[i].reg_num << 2;
                                break;
                            case label:
                                exists_symbol = trie_exists(objfile->symbols_names, ast.op_or_dir.op_line.op_content[i].label_name);
                                if(exists_symbol){
                                    machine_word = exists_symbol->address;
                                }
                                just_inserted = (objfile->code_image, &machine_word);
                                if(!exists_symbol){
                                    /* incase no symbol found, save the pointer to update the adress later */
                                    strcpy(m_symbol.name, ast.op_or_dir.op_line.op_content[i].label_name);
                                    m_symbol.address = just_inserted;
                                    vector_insert(objfile->missing_symbols_table, &m_symbol);
                                }
                                break;
                            default:
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

/* XXXXXXXXXXXX 
   101000000000*/


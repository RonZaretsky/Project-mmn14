#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"
#include "../lexer/lexer.h"
#include "../preprocessor/preprocessor.h"
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include "../global/defines.h"
#include "../global/typedefs.h"
#include "../output/output.h"



typedef struct missing_symbol{
    char name[MAX_SYMBOL_LENGTH];
    unsigned int call_line;
    unsigned int call_address;
    unsigned int * address;
} missing_symbol;




/* Signatures of methods */
static int compile(FILE * file, object_file * objfile, const char* file_name);
static void * ctor_mem_word(const void * copy);
static void dtor_mem_word(void * to_delete);
static void * ctor_symbol(const void * copy);
static void dtor_symbol(void * to_delete);
static void * extern_call_ctor(const void * copy);
static void extern_call_dtor(void * to_delete);
static void * ctor_missing_symbol(const void * copy);
static void dtor_missing_symbol(void * to_delete);
static void add_extern(Vector extern_calls, const char * extern_name, const unsigned int address);
static object_file create_new_objfile(void);
static void destroy_objfile(object_file * objfile);

/* Main function of assembler*/
int assemble(int file_count, char **file_names){
    int i;
    FILE * am_file;
    object_file objfile;
    char am_file_name[MAX_STRING_LENGTH+1] = {0};
    for(i = 0; i < file_count; i++){
        if(preprocesses_file(file_names[i])){
            strcpy(am_file_name,AM_FILES_PATH);
            strcat(am_file_name,file_names[i]);
            strcat(am_file_name,AM_FILE_EXTENSION);
            am_file = fopen(am_file_name, "r");
            if(am_file){
                objfile = create_new_objfile();
                if(compile(am_file, &objfile, am_file_name)){
                    output(file_names[i], &objfile);
                }
                fclose(am_file);
                destroy_objfile(&objfile);
            }
        }
        
    }
    return 0;
}


/* Implemention of methods */

/* compile method */
static int compile(FILE * file, object_file * objfile, const char* file_name){
    missing_symbol m_symbol;
    Vector missing_symbols_table = new_vector(ctor_missing_symbol, dtor_missing_symbol);
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
    const char * data_str;
    void * const * begin;
    void * const * end;

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
                        printf(RED "Error: symbol redefenition" reset);
                        has_error = TRUE;
                    }
                    else {
                        exists_symbol->type = symbol_entry_code;
                        exists_symbol->address = vector_get_items_count(objfile->code_image) + 100;
                    }
                }
                else {
                    current_symbol.type = symbol_code;
                    current_symbol.address = vector_get_items_count(objfile->code_image) + 100;
                    current_symbol.declared_line = line_cnt;
                    trie_insert(objfile->symbols_names, current_symbol.name, vector_insert(objfile->symbol_table, &current_symbol));
                }
            } 
            /* if symbol is directive */
            else {
                if(ast.op_or_dir.dir_line.dir_type <= dir_entry){
                    
                } else {
                    if(exists_symbol){
                        if(ast.op_or_dir.dir_line.dir_type >= dir_string){
                            if(exists_symbol->type != symbol_entry ){
                                /* ERROR REDEF */
                                printf(RED "Error: symbol redefenition" reset);
                                has_error = TRUE;
                            }
                            else {
                                exists_symbol->type = symbol_entry_data;
                                exists_symbol->address = vector_get_items_count(objfile->data_image) + 100;
                                exists_symbol->declared_line = line_cnt;
                            }
                        }
                    }
                    else {
                        if(ast.op_or_dir.dir_line.dir_type >= dir_string){
                            current_symbol.type = symbol_data;
                            current_symbol.address = vector_get_items_count(objfile->data_image) + 100;
                            current_symbol.declared_line = line_cnt;
                            trie_insert(objfile->symbols_names, current_symbol.name, vector_insert(objfile->symbol_table, &current_symbol));
                        }


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
            if(ast.op_or_dir.dir_line.dir_type == dir_string){
                for( i = 0, data_str = ast.op_or_dir.dir_line.dir_content.string;*data_str;data_str++){
                    machine_word = *data_str;
                    vector_insert(objfile->data_image, &machine_word);
                }
                machine_word = 0;
                vector_insert(objfile->data_image, &machine_word);
            }
            if(ast.op_or_dir.dir_line.dir_type == dir_data){
                for(i=0; i<ast.op_or_dir.dir_line.dir_content.data.data_count; i++){
                    machine_word = ast.op_or_dir.dir_line.dir_content.data.data[i];
                    vector_insert(objfile->data_image, &machine_word);
                }

            }
            if(ast.op_or_dir.dir_line.dir_type == dir_entry || ast.op_or_dir.dir_line.dir_type == dir_extern){
                exists_symbol = trie_exists(objfile->symbols_names, ast.op_or_dir.dir_line.dir_content.label_name);
                if(exists_symbol){
                    if(ast.op_or_dir.dir_line.dir_type == dir_entry){
                        if(exists_symbol->type == symbol_entry || exists_symbol->type >= symbol_entry_code){
                            /* ERROR REDEF */
                            printf(RED "Error: symbol redefenition" reset);
                            has_error = TRUE;
                        }
                        else if(exists_symbol->type == symbol_extern){
                            /* ERROR dec as extern and now dec as entry */
                            printf(RED "Error: Declared as extern and now declared as entry" reset);
                            has_error = TRUE;
                        } else {
                            if(exists_symbol->type == symbol_code){
                                exists_symbol->type = symbol_entry_code;
                            } else {
                                exists_symbol->type = symbol_entry_data;

                            }
                        }
                    }else{
                        if(exists_symbol->type == symbol_extern){
                            /* ERROR REDEF */
                            printf(RED "Error: symbol redefenition" reset);
                            has_error = TRUE;
                        } else{
                            /* ERROR dec as !extern and now dec as extern */
                            printf(RED "Error: not declared as extern and now declared as extern" reset);
                            has_error = TRUE;
                        }
                    }
                }else{
                    strcpy(current_symbol.name, ast.op_or_dir.dir_line.dir_content.label_name);
                    current_symbol.type = ast.op_or_dir.dir_line.dir_type;
                    current_symbol.declared_line = line_cnt;
                    current_symbol.address = 0;
                    trie_insert(objfile->symbols_names, current_symbol.name, vector_insert(objfile->symbol_table, &current_symbol));
                }
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
                                /* if symbol exists and is not entry */
                                if(exists_symbol && exists_symbol->type != symbol_entry){
                                    machine_word = exists_symbol->address << 2;
                                    /* if symbol is external insert it to table*/
                                    if(exists_symbol->type == symbol_extern ){
                                        machine_word |= 1;
                                        extern_call_adress = vector_get_items_count(objfile->code_image) + 100;
                                        add_extern(objfile->extern_symbols_table, exists_symbol->name, extern_call_adress);
                                        /*strcpy(m_symbol.name, exists_symbol->name);
                                        vector_insert(e_call.call_address, &extern_call_adress);*/
                                    } else {
                                        machine_word |= 2;
                                    }
                                }
                                /* insert machine word*/
                                just_inserted = vector_insert(objfile->code_image, &machine_word);
                                /* if symbol doesnt exists and it is entry */
                                if(!exists_symbol || (exists_symbol && exists_symbol->type == symbol_entry)){
                                    /* incase no symbol found, save the pointer to update the adress later */
                                    strcpy(m_symbol.name, ast.op_or_dir.op_line.op_content[i].label_name);
                                    m_symbol.address = just_inserted;
                                    m_symbol.call_line = line_cnt;
                                    m_symbol.call_address = vector_get_items_count(objfile->code_image) + 100 -1;
                                    vector_insert(missing_symbols_table, &m_symbol);
                                }
                                break;
                            case none:
                                break;
                        }
                        
                    }

                }
            } else {
                /* this is for rst and stop op codes but it does not have operands so it is empty */
            }
        }
        line_cnt++;
        lexer_trie_deinit();
    }
    
    VECTOR_FOR_EACH(begin,end,objfile->symbol_table){
        if(*begin){
            if(((symbol*)(*begin))->type == symbol_entry){
                /* Error entry x was dec in line y but was never defined in am file*/
                printf(RED "Error: entry %s was decalred but was never defined" reset, ((symbol*)(*begin))->name);
                has_error = TRUE;
            }
            else if(((symbol*)(*begin))->type == symbol_entry_code){
                objfile->entries_count++;
            }
        }
    }

    VECTOR_FOR_EACH(begin,end,missing_symbols_table){
        if(*begin){
            exists_symbol = trie_exists(objfile->symbols_names, ((missing_symbol*)(*begin))->name);
            if(exists_symbol && exists_symbol->type != symbol_entry){
                *(((missing_symbol*)(*begin))->address) = exists_symbol->address << 2;
                if(exists_symbol->type == symbol_extern){
                    *(((missing_symbol*)(*begin))->address) |= 1;
                    add_extern(objfile->extern_symbols_table,exists_symbol->name, ((missing_symbol*)(*begin))->call_address);
                } else {
                    *(((missing_symbol*)(*begin))->address) |= 2;
                }
            } else {
                /* error missing label */
                printf(RED "Error: missing label" reset);
                has_error = TRUE;
            }
        }
    }

    vector_destroy(&missing_symbols_table);
    
    return !has_error;
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
/* this is a constructor for extern call vectors*/
static void * extern_call_ctor(const void * copy){
    return memcpy(malloc(sizeof(extern_call)), copy, sizeof(extern_call));

}
/* this is a distructor for extern call vectors*/
static void extern_call_dtor(void * to_delete){
    extern_call * e_call = to_delete;
    vector_destroy(&(e_call->call_address));
    free(e_call);

}
/* this is a constructor for missing symbol vectors*/
static void * ctor_missing_symbol(const void * copy){
    return memcpy(malloc(sizeof(missing_symbol)), copy, sizeof(missing_symbol));
}
/* this is a distructor for missing symbol vectors*/
static void dtor_missing_symbol(void * to_delete){
    free(to_delete);
}
/* adds new extern if not exists */
static void add_extern(Vector extern_calls, const char * extern_name, const unsigned int address){
    void * const * begin;
    void * const * end;
    extern_call e_call = {0};
    /* check if extern name already exists */
    VECTOR_FOR_EACH(begin,end,extern_calls){
        if(*begin){
            if(strcmp(extern_name, ((const extern_call *)(*begin))->extern_name) == 0){
                vector_insert(((extern_call *)(*begin))->call_address, &address);
                return;
            }
        }
    }
    /* if not exists, add it */
    strcpy(e_call.extern_name, extern_name);
    e_call.call_address = new_vector(ctor_mem_word, dtor_mem_word);
    vector_insert(e_call.call_address, &address);
    vector_insert(extern_calls, &e_call);
}
/*returns new objfile*/
static object_file create_new_objfile(void){
    object_file objfile = {0};
    objfile.code_image = new_vector(ctor_mem_word, dtor_mem_word);
    objfile.data_image = new_vector(ctor_mem_word, dtor_mem_word);
    objfile.symbol_table = new_vector(ctor_symbol, dtor_symbol);
    objfile.extern_symbols_table = new_vector(extern_call_ctor, extern_call_dtor);
    objfile.symbols_names = trie(); 
    return objfile;
}
/* destroys objfile*/
static void destroy_objfile(object_file * objfile){
    vector_destroy(&(objfile->code_image));
    vector_destroy(&(objfile->data_image));
    vector_destroy(&(objfile->symbol_table));
    vector_destroy(&(objfile->extern_symbols_table));
    trie_destroy(&(objfile->symbols_names));
}








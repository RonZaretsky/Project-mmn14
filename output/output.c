#include "output.h"
#include "../data_structures/vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void handle_ent_file(const char * file_name, Vector symbol_table);
static void handle_ext_file(const char * file_name, Vector extern_symbols_table);
static void handle_ob_file(const char * file_name, const object_file *objfile);
static void handle_section_to_64bit(FILE* file, Vector section);


void output(const char * base_name, const object_file *objfile){
    char ext_file_name[MAX_STRING_LENGTH+1] = {0};
    char ent_file_name[MAX_STRING_LENGTH+1] = {0};
    char ob_file_name[MAX_STRING_LENGTH+1] = {0};
    strcpy(ext_file_name,EXT_FILES_PATH);
    strcat(ext_file_name,base_name);
    strcat(ext_file_name,EXT_FILE_EXTENSION);
    strcpy(ent_file_name,ENT_FILES_PATH);
    strcat(ent_file_name,base_name);
    strcat(ent_file_name,ENT_FILE_EXTENSION );
    strcat(ob_file_name,OB_FILES_PATH);
    strcat(ob_file_name,base_name);
    strcat(ob_file_name,OB_FILE_EXTENSION);
    if(objfile->entries_count > 0){
        handle_ent_file(ent_file_name, objfile->symbol_table);
    }
    if(vector_get_items_count(objfile->extern_symbols_table)){
        handle_ext_file(ext_file_name, objfile->extern_symbols_table);
    }
    handle_ob_file(ob_file_name, objfile);
}

static void handle_section_to_64bit(FILE* file, Vector section){
    void * const * begin;   
    void * const * end;
    unsigned int msb_lsb[2];
    int i;
    VECTOR_FOR_EACH(begin,end,section){
        if(*begin){
            msb_lsb[0] = (*(unsigned int *)(*begin) >> 6) & 0x3F;
            msb_lsb[1] = (*(unsigned int *)(*begin)) & 0x3F;
            for(i = 0; i < 2;i++){
                if(msb_lsb[i] >= 0 && msb_lsb[i] <= 25){
                    fprintf(file, "%c", msb_lsb[i] + 'A');
                } else if(msb_lsb[i] >= 26 && msb_lsb[i] <= 51){
                    fprintf(file, "%c", msb_lsb[i] + 'a' - 26);
                } else if(msb_lsb[i] >= 52 && msb_lsb[i] <= 61){
                    fprintf(file, "%c", msb_lsb[i] + '0' - 52);
                } else if(msb_lsb[i] == 62){
                    fprintf(file, "%c", '+');
                } else if(msb_lsb[i] == 63){
                    fprintf(file, "%c", '/');
                }
            }
            fprintf(file, "\n");
        }
    }
}

static void handle_ob_file(const char * file_name, const object_file *objfile){
    FILE * ob_file;
    ob_file = fopen(file_name, "w");
    if(ob_file){
        fprintf(ob_file, "%lu %lu\n", vector_get_items_count(objfile->code_image), vector_get_items_count(objfile->data_image));
        handle_section_to_64bit(ob_file, objfile->code_image);
        handle_section_to_64bit(ob_file, objfile->data_image);



        fclose(ob_file);
    }
}

static void handle_ext_file(const char * file_name, Vector extern_symbols_table){
    FILE * ext_file;
    void * const * begin_ext;
    void * const * end_ext;
    void * const * begin_addr;
    void * const * end_addr;
    ext_file = fopen(file_name, "w");
    if(ext_file){
        VECTOR_FOR_EACH(begin_ext,end_ext,extern_symbols_table){
            if(*begin_ext){
                VECTOR_FOR_EACH(begin_addr,end_addr,((const extern_call*)(*begin_ext))->call_address){
                    if(*begin_addr){
                        fprintf(ext_file, "%s\t%u\n", ((const extern_call*)(*begin_ext))->extern_name, *((const unsigned int*)(*begin_addr)));
                    }
                }
            }
        }
        fclose(ext_file);
    }
}

static void handle_ent_file(const char * file_name, Vector symbol_table){
    FILE * ent_file;
    void * const * begin;
    void * const * end;
    ent_file = fopen(file_name, "w");
    if(ent_file){
        VECTOR_FOR_EACH(begin,end,symbol_table){
            if(*begin){
                if(((symbol*)(*begin))->type >= symbol_entry_code){
                    fprintf(ent_file, "%s\t%u\n", ((symbol*)(*begin))->name, ((symbol*)(*begin))->address);
                }
            }
        }
        fclose(ent_file);
    }
}

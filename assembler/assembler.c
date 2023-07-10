#include "assembler.h"
#include "../lexer/lexer.h"
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"
#include <stdio.h>
#include "../global/defines.h"


typedef struct symbol{
    enum{
        symbol_extern,
        symbol_entry,
        symbol_code,
        symbol_data,
        symbol_entry_code,
        symbol_entry_data
    } type;
    int address;
    char * name;
} symbol;

typedef struct object_file{
    Vector code_section;
    Vector data_section;
    Vector symbol_table;
    Trie symbols_names;
} object_file;

static int compile(FILE * file, object_file * objfile);

int assemble(int file_count, char **file_names){
    return 0;
}

static int compile(FILE * file, object_file * objfile){
    char line[MAX_LINE_LENGTH] = {0};
    assembler_ast line_ast;
    while(fgets(line, MAX_LINE_LENGTH, file)){
        line_ast = line_to_ast_lexer(line);
        if(line_ast.line_type == syntax_error){
            printf(RED "Syntax error: %s\n" reset, line_ast.error_msg);
            return FAILURE;
        } else if (line_ast.line_type == dir){
            

        } else if (line_ast.line_type == op){

        }
    }
    
    
    return 0;
}


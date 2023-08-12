#include "lexer.h"
#include <stdio.h>
#include <string.h>
int main(){
    char line_buffer[100] = {0};
    char save[100];
    int line = 1;
    assembler_ast ast;
    while(fgets(line_buffer, sizeof(line_buffer),stdin)){
        strcpy(save, line_buffer);
        ast = line_to_ast_lexer(line_buffer);
        line++;
    }
    return 0;
}
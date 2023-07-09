#ifndef __LEXER_H__
#define __LEXER_H__
#include "../global/defines.h"


typedef struct assembler_ast{
    char error_msg[MAX_STRING_LENGTH];   
    enum {
        OP,
        DIR
    } line_type;
    union {
        struct {
            enum{
                dir_extern,
                dir_entry,
                dir_string,
                dir_data,
            } dir_type;
            union {
                char *label_name;
                char *string;
                struct{
                    int data[MAX_LINE_LENGTH];
                    int data_count;
                } data;
            } dir_content;
        } dir_line;
        struct {
            enum{
                /* 2 operands */
                op_mov,
                op_cmp,
                op_add,
                op_sub,
                op_lea,

                /* 1 operand */
                op_not,
                op_clr,
                op_inc,
                op_dec,
                op_jmp,
                op_bne,
                op_red,
                op_prn,
                op_jsr,

                /* 0 operands */
                op_rts,
                op_stop,
            } op_type;
            union{
                int const_num;
                int reg_num;
                char *label_name;
            } op_content[2];
        } op_line;
    } op_or_dir;
} assembler_ast;

assembler_ast line_to_ast_lexer(char *line);

#endif /* __LEXER_H__ */
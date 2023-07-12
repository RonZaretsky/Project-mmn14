#ifndef __LEXER_H__
#define __LEXER_H__
#include "../global/defines.h"


typedef struct assembler_ast{
    char error_msg[MAX_STRING_LENGTH];
    char label_name[MAX_SYMBOL_LENGTH];
    enum {
        op,
        dir,
        syntax_error
    } line_type;
    union {
        struct {
            enum{
                dir_extern,
                dir_entry,
                dir_string,
                dir_data
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
                op_lea = 6,

                /* 1 operand */
                op_not = 4 ,
                op_clr = 5,
                op_inc = 7,
                op_dec,
                op_jmp,
                op_bne,
                op_red,
                op_prn,
                op_jsr,

                /* 0 operands */
                op_rts,
                op_stop
            } op_type;
            enum {
                none = 0,
                number = 1,
                label = 3,
                reg = 5
            } op_operand_option[2];
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
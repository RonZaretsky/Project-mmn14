#ifndef __DIR_INS_NAMES_H_
#define __DIR_INS_NAMES_H_

#include "defines.h"

char * directives[DIRECTIVES_COUNT] = {
    ".string",
    ".data",
    ".extern",
    ".entry"
};

char * instructions[INSTRUCTIONS_COUNT] = {
    "mov",
    "cmp",
    "add",
    "sub",
    "lea",
    "clr",
    "not",
    "inc",
    "dec",
    "jmp",
    "bne",
    "red",
    "prn",
    "jsr",
    "rts",
    "stop"
};

#endif /* __DIR_INS_NAMES_H_ */
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__
#include "defines.h"
#include "../data_structures/vector/vector.h"
#include "../data_structures/trie/trie.h"

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
    unsigned int declared_line;
    char name[MAX_SYMBOL_LENGTH+1];
} symbol;

/* an object file struct */
typedef struct object_file{
    Vector code_image;
    Vector data_image;
    Vector symbol_table;
    Vector extern_symbols_table;
    Trie symbols_names;
    int entries_count;
} object_file;






#endif /* __TYPEDEFS_H__ */

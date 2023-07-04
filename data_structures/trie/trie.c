/**
 * @file trie.c
 * @author Ron Zaretsky (ronz2512@icloud.com)
 * @brief This is implementation of trie in c
 * @version 0.1
 * @date 2023-06-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "trie.h"
#include <stdlib.h>

#define TRIE_BASE_CHAR ' '
#define END_OF_STRING '\0'

struct trie_node{
    void *end_of_str_context;
    struct trie_node *next[95];
};

struct trie{
    struct trie_node *next[95];
};

Trie trie(){
    return calloc(1, sizeof(struct trie));
}

static struct trie_node *internal_trie_exists(struct trie_node * node_i,const char * string) {
    while(node_i) {
        if(*string == END_OF_STRING && node_i->end_of_str_context != NULL) {
            return node_i;
        }
        node_i = node_i->next[(*string) - TRIE_BASE_CHAR];
        string++;
    }
    return NULL;
}

void *trie_exists(Trie trie, const char *string){
    struct trie_node *found_node;
    if(string == NULL) return NULL;
    found_node = internal_trie_exists(trie->next[(*string) - TRIE_BASE_CHAR], string+1);
    return found_node == NULL ? NULL : found_node->end_of_str_context;
}

void trie_delete(Trie trie, const char *string){
    struct trie_node *found_node;
    if(string == NULL) return;
    found_node = internal_trie_exists(trie->next[(*string) - TRIE_BASE_CHAR], string+1);
    if(found_node) found_node->end_of_str_context = NULL;
}

const char *trie_insert(Trie trie,const char *string,void * end_of_str_context) {
    struct trie_node ** iterator = &trie->next[(*string) - TRIE_BASE_CHAR];
    char *string_to_insert = (char*)string;
    while(1) {
        if(*iterator == NULL) {
            (*iterator) = calloc(1,sizeof(struct trie_node));
            if(*iterator == NULL)
                return NULL;
        }
        string_to_insert++;
        if(*string_to_insert !='\0')
            iterator = &(*iterator)->next[(*string_to_insert) - TRIE_BASE_CHAR];
        else
            break;
    }
    (*iterator)->end_of_str_context = end_of_str_context;
    return string;
}

static void trie_destroy_sub(struct trie_node * node_i) {
    int i;
    for(i=0;i<95;i++) {
        if(node_i->next[i] != NULL) {
            trie_destroy_sub(node_i->next[i]);
            node_i->next[i] = NULL;
        }
    }
    free(node_i);
}

void trie_destroy(Trie * trie) {
    int i;
    if(*trie != NULL) {
        Trie t = *trie;
        for(i=0;i<95;i++) {
            if(t->next[i] != NULL) 
                trie_destroy_sub(t->next[i]);
        }
        free(*trie);
        (*trie) = NULL;
    }
}



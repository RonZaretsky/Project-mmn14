/**
 * @file trie.h
 * @author Ron Zaretsky (ronz2512@icloud.com)
 * @brief this is header file for implementation of trie in c
 * @version 0.1
 * @date 2023-06-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __TRIE_H_
#define __TRIE_H_


typedef struct trie * Trie;



/**
 * @brief Creates a new trie struct.
 * 
 * @return Trie 
 */
Trie trie();


/**
 * @brief Inserts a string to the trie.
 * 
 * @param trie Trie struct
 * @param string The string you want to insert
 * @param end_of_str_context A pointer to the end of the string
 * @return const char* 
 */
const char *trie_insert(Trie trie, const char *string, void *end_of_str_context);

/**
 * @brief Checks if a string exists in the trie.
 * 
 * @param trie Trie struct
 * @param string The string you want to check
 * @return void* 
 */
void *trie_exists(Trie trie, const char *string);

/**
 * @brief Deletes a string from the trie.
 * 
 * @param trie Trie struct
 * @param string The string you want to delete
 */
void trie_delete(Trie trie, const char *string);

/**
 * @brief Destroys the trie struct.
 * 
 * @param trie Trie struct
 */
void trie_destroy(Trie *trie);


#endif
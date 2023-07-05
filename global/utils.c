#include "defines.h"
#include <stdio.h>

char* continue_till_spaces(char *str){
    int i = 0;
    while(str[i] != ' ' && str[i] != '\t'){
        i++;
    }
    return str + i;
}

char* get_first_word_in_str(char* str){
    char *first_word;
    int i = 0;
    str = continue_till_spaces(str);
    while(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\0'){
        i++;
    }
    first_word = (char*)malloc(sizeof(char) * (i + 1));
    if(first_word == NULL){
        return NULL;
    }
    strncpy(first_word, str, i);
    first_word[i] = '\0';
    return first_word;
}





int check_if_word_in_array(char *word, char *array[], int array_size){
    int i;
    for(i = 0; i < array_size; i++){
        if(strcmp(word, array[i]) == 0){
            return TRUE;
        }
    }
    return FALSE;
}
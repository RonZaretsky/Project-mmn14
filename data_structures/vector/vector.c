#include "vector.h"
#include <stdlib.h>
#include <string.h>

#define VECTOR_BEGIN_SIZE 12

struct vector{
    void **items;
    size_t items_count;
    size_t capacity;
    void * (*ctor)(const void *copy);
    void (*dtor)(void *item);
};

Vector new_vector(void * (*ctor)(const void *copy),  void (*dtor)(void *item)){
    Vector new = calloc(1,sizeof(struct vector)); 
    if(new == NULL) return NULL;
    new->capacity = VECTOR_BEGIN_SIZE;
    new->items = calloc(VECTOR_BEGIN_SIZE, sizeof(void *));
    if(new->items == NULL){
        free(new);
        return NULL;
    }
    new->ctor = ctor;
    new->dtor = dtor;

    return new;
}

size_t vector_get_item_count(const Vector v){
    return v->items_count;
}


void vector_destroy(Vector * v){
    size_t it;
    if((*v)->dtor != NULL){
        for(it = 0; it < (*v)->capacity; it++){
            (*v)->dtor((*v)->items[it]);
        }
    }
    free((*v)->items);
    free(*v);
    *v = NULL;
}





void * vector_insert(Vector v,const void *copy_item ){
    size_t iterator;
    void **temp;
    if(v->capacity == v->items_count){
        v->capacity *= 2;
        temp = realloc(v->items, v->capacity * sizeof(void*));
        if(temp != NULL){
            v->items = temp;
        }else{
            v->capacity /= 2;
            return NULL; 
        }
        v->items[v->items_count] = v->ctor(copy_item);
        if(v->items[v->items_count] == NULL){
            return NULL;
        }
        v->items_count++;
        memset(&v->items[v->items_count],0, v->capacity - v->items_count + 1);
    }else{
        for(iterator = 0; iterator < v->capacity ; iterator++){
            if(v->items[iterator] == NULL){
                v->items[iterator] = v->ctor(copy_item);
                if(v->items[iterator]!= NULL){
                    v->items_count++;
                    break;
                }
                return NULL;
            }
        }
    }

    
    return v->items[iterator];

}


void * const * vector_begin(const Vector v){
    return v->items;
}

void * const * vector_end(const Vector v){
    return &v->items[v->capacity  - 1]; 
}
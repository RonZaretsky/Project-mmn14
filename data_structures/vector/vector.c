#include "vector.h"
#include <stdlib.h>
#include <string.h>

#define VECTOR_BEGIN_SIZE 10

struct vector{
    void **items;
    size_t items_count;
    size_t capacity;
    void * (*ctor)(const void *copy);
    void (*dtor)(void *item);
};

Vector new_vector(void * (*ctor)(const void *copy),void (*dtor)(void *item)) {
    Vector new = calloc(1,sizeof(struct vector));
    if(new == NULL)
        return NULL;
    new->capacity = VECTOR_BEGIN_SIZE;
    new->items = calloc(VECTOR_BEGIN_SIZE,sizeof(void*));
    if(new->items == NULL) {
        free(new);
        return NULL;
    }
    new->ctor = ctor;
    new->dtor = dtor;
    return new;
}

size_t vector_get_items_count(const Vector v){
    return v->items_count;
}


void vector_destroy(Vector * v) {
    size_t it;
    if(*v != NULL) {
        if((*v)->dtor != NULL) {
            for(it = 0;it < (*v)->capacity; it++) {
                if((*v)->items[it] != NULL)
                    (*v)->dtor((*v)->items[it]);
            }
        }
        free((*v)->items);
        free(*v);
        *v = NULL;
    }
}





void * vector_insert(Vector v,const void * copy_item) {
    size_t it;
    void ** temp;
    if(v->items_count == v->capacity) {
        v->capacity *=2;
        temp = realloc(v->items,v->capacity * sizeof(void *));
        if(temp == NULL) {
            v->capacity /= 2;
            return NULL;
        }
        v->items = temp;
        v->items[v->items_count] = v->ctor(copy_item);
        if(v->items[v->items_count] ==NULL) {
            return NULL;
        }
        v->items_count++;
        /*
        memset(&v->items[v->items_count],0,(v->capacity - v->items_count) + 1);
        */
        for(it = v->items_count; it < v->capacity; it++) {
            v->items[it] = NULL;
        }
    }else {
        for(it = 0;it<v->capacity;it++) {
            if(v->items[it] == NULL) {
                v->items[it] = v->ctor(copy_item);
                if(v->items[it] != NULL) {
                    v->items_count++;
                    break;    
                }
                return NULL;
            }
        }
    }
    return v->items[v->items_count-1];
}



void * const * vector_begin(const Vector v){
    return v->items;
}

void * const * vector_end(const Vector v){
    return &v->items[v->capacity  - 1]; 
}

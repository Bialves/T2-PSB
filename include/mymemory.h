#ifndef MYMEMORY_H
#define MYMEMORY_H
#include <stddef.h>

typedef struct allocation 
{
    void *start;
    size_t size;
    struct allocation *next;
    void *end; // Ptr do fim do bloco
} allocation_t;


typedef struct 
{
    void *pool; // ponteiro para o bloco de memória real
    size_t total_size;
    allocation_t *head; // ponteiro para a lista encadeada
} mymemory_t;


mymemory_t* mymemory_init(size_t size);
void* mymemory_alloc(mymemory_t *memory, size_t size);
void mymemory_free(mymemory_t *memory, void *ptr);
void mymemory_display(mymemory_t *memory);
void mymemory_stats(mymemory_t *memory);
void mymemory_cleanup(mymemory_t *memory);
#endif
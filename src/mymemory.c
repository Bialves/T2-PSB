#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/mymemory.h"

// Implementations
mymemory_t* mymemory_init(size_t size) 
{
    mymemory_t *memory = (mymemory_t*)malloc(sizeof(mymemory_t));
    // Falha na alocação de memória para mymemory_t
    if (memory == NULL) {
        return NULL;
    }

    memory->pool = malloc(size);
    // Falha na alocação de memória para o pool
    if (memory->pool == NULL) {
        free(memory);
        return NULL;
    }

    memory->total_size = size;
    memory->head = NULL;
    return memory;
}

void* mymemory_alloc(mymemory_t *memory, size_t size);
void mymemory_free(mymemory_t *memory, void *ptr);
void mymemory_display(mymemory_t *memory);
void mymemory_stats(mymemory_t *memory);
void mymemory_compact(mymemory_t *memory);
void mymemory_cleanup(mymemory_t *memory);
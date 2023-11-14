#include <stddef.h>
#include <cstddef>

typedef struct allocation 
{
    void *start;
    size_t size;
    struct allocation *next;
} allocation_t;


typedef struct 
{
    void *pool; // ponteiro para o bloco de memória real
    size_t total_size;
    allocation_t *head;
} mymemory_t;
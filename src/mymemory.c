#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/mymemory.h"

// Implementations
mymemory_t *mymemory_init(size_t size)
{
   mymemory_t *memory = (mymemory_t *)malloc(sizeof(mymemory_t));
   // Falha na alocação de memória para mymemory_t
   if (memory == NULL) {
      return NULL;
   }

   memory->pool = malloc(size);
   // Falha na alocação de memória para o pool
   if (memory->pool == NULL) {
      free(memory); // Libera a alocação para mymemory_t
      return NULL;
   }

   memory->total_size = size;
   memory->head = NULL;
   return memory;
}

void *mymemory_alloc(mymemory_t *memory, size_t size);
void mymemory_free(mymemory_t *memory, void *ptr)
{
   /*
    * Necessário realizar um compact antes
    * de percorrer, para evitar cair em gaps
    */
   allocation_t *current = memory->head; // Começo da lista
   allocation_t *prev = NULL; // Var auxiliar para trabalhar referências

   // Percorre a lista em busca do endereço informado
   while (current != NULL) {
      // Se encontrou o endereço...
      if (current->start == ptr)
      {
         if (prev != NULL)
         { // Se prev não é null
            prev->next = current->next;
         }
         else
         { // Senão, remoção do inicio
            memory->head = current->next;
         }

         free(current); // Por fim, libera o endereço
         return;
      }
      // Atualiza as referências para avançar na lista
      prev = current;
      current = current->next;
   }
}

void mymemory_display(mymemory_t *memory)
{
   /*
    * Necessário realizar um compact antes
    * de percorrer, para evitar cair em gaps
    */
   allocation_t *current = memory->head; // Começo da lista
   // Enquanto houver blocos alocados...
   while (current != NULL) {
      printf("Start: %p, Size: %zu\n", current->start, current->size);
      current = current->next;
   }
}

void mymemory_stats(mymemory_t *memory);

void mymemory_cleanup(mymemory_t *memory)
{
   /*
    * Necessário realizar um compact antes
    * de limpar, para evitar cair em gaps
    */

   allocation_t *current = memory->head; // Começo da lista
   // Enquanto houver blocos alocados...
   while (current != NULL) {
      allocation_t *next = current->next; // next recebe o próximo
      free(current); // O bloco atual é liberado
      current = next;
   }
   free(memory); // Por fim, a memória total é liberada
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/mymemory.h"

// Implementations
mymemory_t *mymemory_init(size_t size)
{
   if (size == 0) { // Argumento inválido
      return NULL;
   }

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

void *mymemory_alloc(mymemory_t *memory, size_t size)
{
   if (size == 0 || size > memory->total_size) { // Argumento inválido
      return NULL; 
   }

   allocation_t *best_block = NULL; // Var auxiliar
   allocation_t *current = memory->head; // Usado para percorrer a lista

   // Percorre a lista
   while (current != NULL) {
      if (current->size >= size) {
         // Verifica se o bloco é suficientemente grande
         if (best_block == NULL || current->size < best_block->size) {
            best_block = current;
         }
      }
      current = current->next;
   }

   if (best_block != NULL) { // Bloco encontrado, aloca
      best_block->start = malloc(size);
      best_block->size = size;
      return best_block->start;
   }

   return NULL; // Não há bloco livre suficientemente grande
}

void mymemory_free(mymemory_t *memory, void *ptr)
{
   if (memory == NULL || ptr == NULL) {
      printf("ERROR: invalid arguments");
      return;
   }

   allocation_t *current = memory->head; // Começo da lista
   allocation_t *prev = NULL; // Var auxiliar para trabalhar referências

   // Percorre a lista em busca do endereço informado
   while (current != NULL) {
      // Se encontrou o endereço...
      if (current->start == ptr) {
         if (prev != NULL) { // Se prev não é null
            prev->next = current->next;
         }
         else { // Senão, remoção do inicio
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
   if (memory == NULL) {
      printf("ERROR: invalid arguments");
      return;
   }

   allocation_t *current = memory->head; // Começo da lista
   // Enquanto houver blocos alocados...
   while (current != NULL) {
      printf("Start: %p, Size: %zu\n", current->start, current->size);
      current = current->next;
   }
}

void mymemory_stats(mymemory_t *memory)
{
   if (memory == NULL) {
      printf("ERROR: invalid arguments");
      return;
   }

   // Var para as informações
   size_t total_allocations = 0;
   size_t total_allocated_memory = 0;
   size_t total_free_memory = 0;
   size_t largest_block = 0;
   size_t fragments = 0;

   allocation_t *current = memory->head; // Começo da lista
   // Percorre a lista da memória
   while (current != NULL) {
      if (current->start != NULL) { // Bloco alocado
         total_allocations++;
         total_allocated_memory += current->size;
      }
      else { // Bloco livre
         total_free_memory += current->size;

         if (current->size > largest_block) {
            largest_block = current->size;
         }

         fragments++;
      }

      current = current->next;
   }

   // Imprime as estatísticas
   printf("Total number of allocations: %zu\n", total_allocations);
   printf("Total memory allocated: %zu bytes\n", total_allocated_memory);
   printf("Total free memory: %zu bytes\n", total_free_memory);
   printf("Largest contiguous block of memory: %zu bytes\n", largest_block);
   printf("Number of free memory fragments: %zu\n", fragments);
}

void mymemory_cleanup(mymemory_t *memory)
{
   if (memory == NULL) {
      printf("ERROR: invalid arguments");
      return;
   }

   allocation_t *current = memory->head; // Começo da lista
   // Enquanto houver blocos alocados...
   while (current != NULL) {
      allocation_t *next = current->next; // next recebe o próximo
      free(current); // O bloco atual é liberado
      current = next;
   }
   free(memory); // Por fim, a memória total é liberada
}
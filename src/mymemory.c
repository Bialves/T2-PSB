#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
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
// Best Fit
void *mymemory_alloc(mymemory_t *memory, size_t size)
{  // Argumentos inválidos
   if (size == 0 || size > memory->total_size) { 
      return NULL; 
   }

   /**
    * -------------------------------------------------------------
    * Alocação no início da lista
   */
   allocation_t *previous = (allocation_t*)malloc(sizeof(allocation_t));
   if (previous == NULL) { // Se a inicialização falhou...
      return NULL;
   }
   allocation_t *current = memory->head; // Usado para percorrer a lista
   if (current == NULL) {
      previous->size = size;
      /**
       * Ptr do início do bloco vai ser o
       * ptr de início do pool da memória
      */
      previous->start = (char *)memory->pool;
      memory->head = previous;
      previous->next = NULL;
      return previous->start;
   }

   /**
    * ------------------------------------------------------------
    * Alocação pelo meio/fim da lista
   */
   size_t best_size = SIZE_MAX; // Inicializa com um valor máx
   char *ptr_prev = (char *)current->start; // Var ptr auxiliar
   allocation_t *prev = NULL; // Var auxiliar
   
   // Percorre a lista...
   while (current != NULL) {
      if (ptr_prev != (char *)current->start) {
         size_t gap = (size_t)((char *)current->start - ptr_prev);
         if (best_size > gap && gap >= size) {
            previous = prev;
            best_size = gap;

            if (best_size == size) { // Bloco do tam ideal
               break;
            }
         }
      }
      // Fim da lista e não encontrou gaps, mas há espaço para armezenar
      if (current->next == NULL && best_size == SIZE_MAX &&
         (size_t)((char *)memory->pool + memory->total_size) - (size_t)((char *)current->start + current->size) >= size) {
         previous = current;
         best_size = (((size_t)(char *)memory->pool) + memory->total_size) - (((size_t)(char *)previous->start) + previous->size);
         break;
      }
      
      ptr_prev = (char *)current->start + current->size; // Ptr para o fim do bloco atual
      prev = current;
      current = current->next;
   }

   if (best_size == SIZE_MAX) { // Não há gap capaz de armazenar o bloco
      free(previous);
      return NULL;
   } else { // Há um gap capaz de armazenar o bloco
      allocation_t *new_allocation = (allocation_t *)malloc(sizeof(allocation_t));
      if (new_allocation == NULL) { // Falha na alocação para o novo bloco
         free(previous);
         return NULL;
      }

      new_allocation->start = (char *)(previous->start + previous->size);
      new_allocation->size = size;
      new_allocation->next = previous->next; // O novo bloco aponta para o bloco seguinte ao previous
      previous->next = new_allocation; // previous aponta para o novo bloco

      // printf("Previous - start: %p, size: %zu, next start: %p\n", previous->start, previous->size, previous->next->start);
      return new_allocation->start;
   }
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
            current->next->start = memory->pool; // Trata remoção do início
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
   char *prev = (char *)current->start; // Var auxiliar
   int num = 0;
   int gap = 0;
   // Enquanto houver blocos alocados...
   while (current != NULL) {
      if (prev != (char *)current->start) { // Identificou um gap de memória livre
         gap++;
         printf("Gap %d - start: %p, size: %zu\n", gap, prev, (size_t)((char *)current->start - prev));
      }
      // Encontrou um bloco contíguo
      num++;
      printf("Block %d - start: %p, size: %zu\n", num, current->start, current->size);
 
      prev = (char *)current->start + current->size;
      current = current->next;
   }
   // Gap do final da memória total
   size_t free = (size_t)(((char *)memory->pool) + memory->total_size) - (size_t)prev;
   if (free > 0) {
      gap++;
      printf("Gap %d - start: %p, size: %zu\n", gap, prev, free);
   }
}

void mymemory_stats(mymemory_t *memory)
{
   if (memory == NULL) {
      printf("ERROR: invalid arguments");
      return;
   }

   // Var para as informações
   int total_allocations = 0;
   size_t total_allocated_memory = 0;
   size_t total_free_memory = memory->total_size;
   size_t largest_block = 0;
   allocation_t *big = (allocation_t*)malloc(sizeof(allocation_t));
   int fragments = 0;

   allocation_t *current = memory->head; // Começo da lista
   // Percorre a lista da memória
   while (current != NULL) {
      total_allocations++;
      total_allocated_memory += current->size;

      /**
       * Se o início do bloco current mais o seu tamanho,
       * não for o início do próximo bloco, há fragmentação...
       * Ou se ele for o último da lista e ainda houver espaço
       * na memória, há fragmentação
      */
      char *block_ptr = ((char *)current->start + current->size);
      size_t end = (size_t)((char *)memory->pool + memory->total_size) - (size_t)block_ptr;
      if (current->next != NULL && ((char *)block_ptr != current->next->start)) {
         fragments++;
         // Verifica o maior bloco
         if (largest_block < (size_t)((char *)current->next->start) - (size_t)block_ptr) {
            largest_block = (size_t)((char *)current->next->start) - (size_t)block_ptr;
            big->start = (char *)block_ptr;
            big->size = largest_block;
         }
      } 
      else if (current->next == NULL && end > 0) {
         fragments++;
         // Verifica o maior bloco
         if (largest_block < end) {
            largest_block = end;
            big->start = (char *)block_ptr;
            big->size = largest_block;
         }
      }

      current = current->next;
   }
   total_free_memory -= total_allocated_memory; // Atualiza o total de memória livre

   // Imprime as estatísticas
   printf("Total number of allocations: %d\n", total_allocations);
   printf("Total memory allocated: %zu bytes\n", total_allocated_memory);
   printf("Total free memory: %zu bytes\n", total_free_memory);
   printf("Largest contiguous block of memory - start: %p, size: %zu bytes\n", big->start, big->size);
   printf("Number of free memory fragments: %d\n", fragments);

   free(big); // Libera a alocação auxiliar
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
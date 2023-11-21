#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "../include/mymemory.h"

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
   if (memory->head == NULL) {
      allocation_t *new_allocation = (allocation_t*)malloc(sizeof(allocation_t));
      if (new_allocation == NULL) { // Se a inicialização falhou...
         return NULL;
      }
      new_allocation->size = size;
      /**
       * Ptr do início do bloco vai ser o
       * ptr de início do pool da memória
      */
      new_allocation->start = (char *)memory->pool;
      new_allocation->end = (char *)new_allocation->start + new_allocation->size;
      new_allocation->next = NULL;
      memory->head = new_allocation;

      return new_allocation->start;
   }

   /**
    * ------------------------------------------------------------
    * Alocação pelo meio/fim da lista
   */
   allocation_t *current = memory->head; // Usado para percorrer a lista
   allocation_t *prev = NULL; // Var auxiliar
   size_t best_size = SIZE_MAX; // Inicializa com um valor máx
   allocation_t *best_block = NULL;
   char *end_prev = (char *)current->start;
   
   // Percorre a lista...
   while (current != NULL) {
      if (end_prev != (char *)current->start) { // Identifica que há um gap
         size_t gap = (size_t)((char *)current->start - end_prev); // Descobre o tam do gap
         if (best_size > gap && gap >= size) { // Valida se o gap é próximo do tam do novo bloco
            best_block = prev;
            best_size = gap;

            if (best_size == size) { // Encontrou bloco do tam ideal
               break;
            }
         }
      }
      // Fim da lista e não encontrou gaps, mas há espaço para armezenar
      if (current->next == NULL && best_size == SIZE_MAX &&
         (size_t)((char *)memory->pool + memory->total_size) - (size_t)current->end >= size) {
         best_block = current;
         best_size = (((size_t)(char *)memory->pool) + memory->total_size) - (size_t)best_block->end;
         break;
      }
      
      end_prev = (char *)current->end; // Ptr para o fim do bloco atual
      prev = current; // Referência para o bloco atual
      current = current->next; // Atualiza para o próximo bloco
   }

   if (best_size == SIZE_MAX) { // Não há gap capaz de armazenar o bloco
      return NULL;
   } else { // Há um gap capaz de armazenar o bloco
      allocation_t *new_allocation = (allocation_t *)malloc(sizeof(allocation_t));
      if (new_allocation == NULL) { // Falha na alocação para o novo bloco
         return NULL;
      }

      new_allocation->start = (char *)best_block->end;
      new_allocation->size = size;
      new_allocation->end = (char *)new_allocation->start + new_allocation->size;
      new_allocation->next = best_block->next; // O novo bloco aponta para o bloco seguinte ao best_block
      best_block->next = new_allocation; // best_block aponta para o novo bloco

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
            // Trata remoção do início
            if (current->next == NULL) { // Se não há próximo...
               memory->head = NULL;
            } 
            else { // Se há...
               // Atualiza a info do novo bloco head
               current->next->start = memory->pool;
               current->next->end = (char *)memory->pool + current->next->size;
               memory->head = current->next;
            }
         }

         free(current); // Por fim, libera o endereço
         printf("Free successfully!\n");
         return;
      }
      // Atualiza as referências para avançar na lista
      prev = current;
      current = current->next;
   }

   printf("ERROR: invalid address!\n");
}

void mymemory_display(mymemory_t *memory)
{
   if (memory == NULL || memory->head == NULL) {
      printf("ERROR: invalid arguments");
      return;
   }

   allocation_t *current = memory->head; // Começo da lista
   char *end_prev = (char *)current->start; // Var auxiliar
   int num = 0;
   int gap = 0;

   // Enquanto houver blocos alocados...
   while (current != NULL) {
      if (end_prev != (char *)current->start) { // Identificou um gap de memória livre
         gap++;
         printf("Gap %d - start: %p, size: %zu\n", gap, end_prev, (size_t)((char *)current->start - end_prev));
      }
      // Encontrou um bloco alocado
      num++;
      printf("Block %d - start: %p, size: %zu\n", num, current->start, current->size);
 
      end_prev = (char *)current->end; // Ptr pro fim do bloco atual
      current = current->next; // Atualiza para o próximo bloco
   }
   // Gap do final da memória total
   size_t free = (size_t)(((char *)memory->pool) + memory->total_size) - (size_t)end_prev;
   if (free > 0) { // Se há...
      gap++;
      printf("Gap %d - start: %p, size: %zu\n", gap, end_prev, free);
   }
}

void mymemory_stats(mymemory_t *memory)
{
   if (memory == NULL || memory->head == NULL) {
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
      char *ptr_end = (char *)current->end;
      size_t memory_end = (size_t)((char *)memory->pool + memory->total_size) - (size_t)ptr_end;
      if ((current->next != NULL && ptr_end != current->next->start)
         || (current->next == NULL && memory_end > 0)
      ) {
         fragments++;

         // Verifica o maior bloco
         size_t aux = 0;
         if (current->next == NULL) {
            aux = memory_end;
         } else {
            aux = (size_t)current->next->start - (size_t)ptr_end;
         }

         if (largest_block < aux) {
            largest_block = aux;
            big->start = ptr_end;
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
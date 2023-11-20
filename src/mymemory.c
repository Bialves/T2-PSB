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
{
   if (size == 0 || size > memory->total_size) { // Argumento inválido
      return NULL; 
   }

   allocation_t *best_block = NULL;
   allocation_t *current = memory->head; // Usado para percorrer a lista
   
   // Primeiro bloco a ser inserido
   if (current == NULL) {
      best_block = (allocation_t*)malloc(sizeof(allocation_t));
      if (best_block == NULL) { // Se a inicialização falhou...
         return NULL;
      }
      best_block->size = size;
      /**
       * Ptr do início do bloco vai ser o
       * ptr de início do pool da memória
      */
      best_block->start = (char *)memory->pool;
      memory->head = best_block;
      best_block->next = NULL;
      return best_block->start;
   }

   size_t best_size = SIZE_MAX; // Inicializa com um valor máx
   size_t free = memory->total_size;

   // Percorre a lista...
   while (current != NULL) {
      free -= current->size;
      if (current->size >= size && current->size < best_size) {
         best_block = current;
         best_size = current->size;
      }
      current = current->next;
   }

   if (best_block == NULL) { // Não blocos capazes de armazenar
      return NULL;
   }

   // Verifica se é possível dividir o bloco para alocar apenas a quantidade necessária
   if (best_size > size + sizeof(allocation_t)) {
      allocation_t *new_allocation = (allocation_t*)malloc(sizeof(allocation_t));
      if (new_allocation == NULL) {
         return NULL; // Falha na alocação para o novo bloco
      }

      new_allocation->start = (char *)best_block->start + size;
      new_allocation->size = best_size - size - sizeof(allocation_t);
      new_allocation->next = NULL;
      best_block->next = new_allocation;
      return new_allocation->start;
   } 

   return NULL;
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
   int num = 0;
   // Enquanto houver blocos alocados...
   while (current != NULL) {
      num++;
      printf("Block %d -  start: %p, size: %zu\n", num, current->start, current->size);
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
   int total_allocations = 0;
   size_t total_allocated_memory = 0;
   size_t total_free_memory = memory->total_size;
   size_t largest_block = total_free_memory;
   allocation_t *block = NULL;
   int fragments = 0;

   allocation_t *current = memory->head; // Começo da lista
   // Percorre a lista da memória
   while (current != NULL) {
      total_allocations++;
      total_allocated_memory += current->size;

      if (current->start == NULL && current->size > largest_block) { // Descobre o maior bloco livre
         largest_block = current->size;
         block = current;
      }

      /**
       * Se o início do bloco current menos o seu tamanho,
       * não for o início do próximo bloco, há fragmentação
      */
      if (current->next != NULL && !((void *)((char *)current->start - current->size) == current->next->start)) {
         fragments++;
      }

      current = current->next;
   }
   total_free_memory -= total_allocated_memory; // Atualiza o total de memória livre

   // Imprime as estatísticas
   printf("Total number of allocations: %d\n", total_allocations);
   printf("Total memory allocated: %zu bytes\n", total_allocated_memory);
   printf("Total free memory: %zu bytes\n", total_free_memory);
   printf("Largest contiguous block of memory\n  start: %p - size: %zu bytes\n", block, largest_block);
   printf("Number of free memory fragments: %d\n", fragments);
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
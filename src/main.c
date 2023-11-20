#include <stdio.h>
#include <stdint.h>
#include "../include/mymemory.h"

int input(); // Protótipo da função auxiliar

int main()
{
   unsigned int choice;
   mymemory_t *mem = NULL; // Var da memória total

   /**
    * Ao executar, primeira ação é inicializar a memória total,
    * caso ocorra falha o programa é encerrado
    */
   size_t size;
   printf("------- WELCOME! -------\n> Memory Init\nEnter a size (bytes): ");
   scanf("%zu", &size);

   mem = mymemory_init(size);
   // Se a inicialização falou...
   if (mem == NULL) {
      printf("ERROR: initialization failure\n");
      return 1; // Encerra execução
   }
   printf("Memory initialization successfully!\nMemory ptrs:\nInit: %p - Pool: %p\n", mem, mem->pool);

   do {
      // Imprime menu de opções
      printf(
         "\n------- Options -------\n"
         "1. Alloc Block\n"
         "2. Free Alloc\n"
         "3. Display\n"
         "4. Memory Statics\n"
         "5. Exit\n"
         "\n> Enter your choice: "
      );
      choice = input();

      switch (choice) {
         case 1: {
            size_t size_alloc;
            printf("> Alloc Block\nEnter a size (bytes): ");
            scanf("%zu", &size_alloc);

            void *block = mymemory_alloc(mem, size_alloc);
            if (block == NULL) {
               printf("\nERROR: Allocation failure!\n");
            } else {
               printf("\nAllocation successfully! Block ptr: %p\n", block);
            }
            break;
         }
         case 2: {
            printf("> Free Alloc\nAddress: ");
            unsigned long ptr;
            scanf("%lx", &ptr);

            mymemory_free(mem, (void *)ptr);
            printf("Free successfully!\n");
            break;
         }
         case 3: {
            printf("> Display\n");
            mymemory_display(mem);
            break;
         }
         case 4: {
            printf("> Memory Statics\n");
            mymemory_stats(mem);
            break;
         }
         case 5: {
            printf(
               "> Exit\n"
               "Cleaning up memory... Bye bye\n"
            );
            break;
         }
         default:
            printf("ERROR: wrong Input\n");
      }
   } while (choice != 5);

   /**
    * Antes de encerrar execução, libera a memória
    * e os blocos alocados nela
    */
   mymemory_cleanup(mem);
   return 0;
}

/**
 * Função auxiliar que recebe
 * entradas númericas do usuário
 */
int input()
{
   unsigned int number;
   scanf("%d", &number);
   return (number);
}

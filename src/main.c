#include <stdio.h>
#include <stdint.h>
#include "../include/mymemory.h"

int input();

int main()
{
	int choice;
    mymemory_t *mem = NULL; // Var da memória total

    /**
     * Ao executar, primeira ação é inicializar a memória total,
     * caso ocorra falha, o programa é encerrado
    */
    size_t size;
    printf("------- WELCOME! -------\n> Memory Init\nEnter a size: ");
    scanf("%zu", &size);

    mem = mymemory_init(size);
    if (mem == NULL) { // Inicialização falou...
        printf("ERROR: initialization failure\n");
        return 1; // Encerra execução
    }
    printf("Memory ptr: %p\n\n", mem);

    do {
        // Imprime menu de opções
        printf("------- Options -------\n");
        printf("1. Alloc Block\n");
        printf("2. Free Alloc\n");
        printf("3. Display\n");
        printf("4. Memory Statics\n");
        printf("5. Compact\n");
        printf("6. Exit\n");
        printf("\n> Enter your choice: ");
        choice = input();

        switch (choice) {
            case 1: {
                printf("> Alloc Block\n--\n");
                // Implementar função
                printf("\nAllocation successfully!\n");
                break;
            }
            case 2: {
                printf("> Free Alloc\nAddress: ");
                unsigned long ptr;  
                scanf("%lx", &ptr);

                mymemory_free(mem, &ptr);
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
                // Implementar função
                break;
            }
            case 5: {
                printf("> Compact\n");
                // Implementar função
                break;
            }
            case 6: {
                printf("Interrupting...\n");
                break;
            }
            default:
                printf("ERROR: wrong Input\n");
        }
    } while (choice != 6);

    // Antes de encerrar execução, libera a memória total
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

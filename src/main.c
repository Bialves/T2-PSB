#include <stdio.h>
#include <stdint.h>
#include "../include/mymemory.h"

int input();

int main()
{
	int choice;

    do {
        // printing menu
        printf("------- Menu -------\n");
        printf("1. Memory Init\n");
        printf("2. Allocation\n");
        printf("3. Free\n");
        printf("4. Display\n");
        printf("5. Stats\n");
        printf("6. Compact\n");
        printf("7. Clean up\n");
        printf("8. Exit\n");
        printf("\nEnter your choice: ");
        
        choice = input();

        switch (choice) {
            case 1: {
                printf("Size: ");
                int size = input();
                
                void *init = mymemory_init(size);
                printf("Ptr: %p\n", init);
                break;
            }
            case 2: {
                printf("TWO\n");
                break;
            }
            case 3: {
                printf("THREE\n");
                break;
            }
            case 4: {
                printf("FOUR\n");
                break;
            }
            case 5: {
                printf("FIVE\n");
                break;
            }
            case 6: {
                printf("SIX\n");
                break;
            }
            case 7: {
                printf("SEVEN\n");
                break;
            }
            case 8: {
                printf("Bye bye\n");
                break;
            }
            default:
                printf("ERR: wrong Input\n");
        }
    } while (choice != 8);

    return 0;
}


int input()
{
	int number;
	scanf("%d", &number);
	return (number);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intro.h"
#include "outro.h"

/* Minimal student struct for future expansion */
typedef struct {
    char name[64];
    char sap[16];
    int marks;
    char batch[8];
} Student;

/* Placeholder functions (implement later) */
void addStudents(void) {
    system("clear");
    printf("Add Students\n");
    printf("Press Enter to return to menu...");
    getchar();
}

void viewDatabase(void) {
    system("clear");
    printf("View Student Database\n");
    printf("Press Enter to return to menu...");
    getchar();
}

void allocateBatches(void) {
    system("clear");
    printf("Allocate Batches\n");
    printf("Press Enter to return to menu...");
    getchar();
}

void viewAllocatedBatches(void) {
    system("clear");
    printf("View Allocated Batches\n");
    printf("Press Enter to return to menu...");
    getchar();
}

void showMenu(void) {
    system("clear");
    printf("\033[1;34m");
    printf("================================================================================\n");
    printf("                            STUDENT BATCH ALLOCATION SYSTEM                      \n");
    printf("================================================================================\n\033[0m");

    printf("   1. Add More Students to Database\n");
    printf("   2. View Student Database\n");
    printf("   3. Allocate Batches\n");
    printf("   4. View Allocated Batches\n");
    printf("   5. Exit Program\n");

    printf("--------------------------------------------------------------------------------\n");
    printf("   Enter your choice: ");
}

int main(void) {
    int choice = 0;

    /* Show intro screen implemented in intro.c */
    showIntro();

    while (1) {
        showMenu();

        if (scanf("%d", &choice) != 1) {
            /* invalid input: flush line and continue */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("\nInvalid input! Press Enter to try again...");
            getchar();
            continue;
        }
        /* consume trailing newline */
        getchar();

        switch (choice) {
            case 1:
                addStudents();
                break;
            case 2:
                viewDatabase();
                break;
            case 3:
                allocateBatches();
                break;
            case 4:
                viewAllocatedBatches();
                break;
            case 5:
                /* call outro from outro.c */
                showThankYou();
                return 0;
            default:
                printf("\nInvalid choice! Press Enter to try again...");
                getchar();
        }
    }

    return 0;
}

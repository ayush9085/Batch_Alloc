#include "outro.h"
#include <stdio.h>
#include <stdlib.h>

void showThankYou(void) {
    system("clear");

    printf("\n\n");

    /* Top border (cyan) */
    printf("\033[1;36m");
    printf("================================================================================\n");
    printf("                                  THANK YOU                                      \n");
    printf("================================================================================\n");
    printf("\033[0m\n");

    /* Message */
    printf("   This project was developed as part of the Mini Project for the\n");
    printf("   \033[1mText Similarity Based Plagiarism Detection System.\033[0m\n\n");

    printf("   Submitted By   : Ayush Pandey\n");
    printf("   Batch          : B10\n");
    printf("   SAP ID         : 590025256\n");
    printf("   College        : UPES, School of Computer Science\n\n");

    /* Footer */
    printf("\033[1;33m");
    printf("================================================================================\n");
    printf("                     Press Enter to exit the program...                          \n");
    printf("================================================================================\n");
    printf("\033[0m");

    getchar();
}

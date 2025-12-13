#include "intro.h"
#include <stdio.h>
#include <stdlib.h>

void showIntro(void) {
    /* Clear terminal (works on macOS / Linux). On Windows use "cls". */
    system("clear");

    /* Top Border & Heading (Cyan + Bold) */
    printf("\033[1;36m");
    printf("================================================================================\n");
    printf("                   STUDENT RECORD MANAGEMENT SYSTEM - BATCH ALLOCATION         \n");
    printf("================================================================================\n\033[0m");

    /* Project Details (Bold labels, aligned neatly) */
    printf("   \033[1mProject Title   \033[0m: Student Record Management System (SRMS)\n");
    printf("   \033[1mSubmitted By    \033[0m: Ayush Pandey\n");
    printf("   \033[1mBatch / SAP ID  \033[0m: B10 / 590025156\n");
    printf("   \033[1mCollege         \033[0m: UPES, School of Computer Science\n\n");

    /* Bottom Section (Yellow) */
    printf("\033[1;33m");
    printf("================================================================================\n");
    printf("                        Press Enter to continue...                              \n");
    printf("================================================================================\n");
    printf("\033[0m");

    /* wait for Enter */
    getchar();
}

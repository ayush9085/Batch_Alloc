#include "intro.h"
#include <stdio.h>
#include <stdlib.h>

void showIntro(void) {
    /* Clear terminal (works on macOS / Linux). On Windows use "cls". */
    system("clear");

    /* Top Border & Heading (Cyan + Bold) */
    printf("\033[1;36m");
    printf("================================================================================\n");
    printf("                   TEXT SIMILARITY PLAGIARISM PROJECT                           \n");
    printf("================================================================================\n\033[0m");

    /* Project Details (Bold labels, aligned neatly) */
    printf("   \033[1mProject Title   \033[0m: Plagiarism Detection System Using Text Similarity\n");
    printf("   \033[1mSubmitted By    \033[0m: Ayush Pandey\n");
    printf("   \033[1mSAP ID          \033[0m: 590025156\n");
    printf("   \033[1mBatch           \033[0m: B10\n");
    printf("   \033[1mCollege         \033[0m: UPES, School of Computer Science\n");
    printf("   \033[1mSubmitted To    \033[0m: Professor Pankaj Badoni\n\n");

    /* Bottom Section (Yellow) */
    printf("\033[1;33m");
    printf("================================================================================\n");
    printf("                        Press Enter to continue...                              \n");
    printf("================================================================================\n");
    printf("\033[0m");

    /* wait for Enter */
    getchar();
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

int t[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int main(int argc, char **argv) {
    int i;
    srand(time(NULL));
    printf("RAND_MAX  = %d\n", RAND_MAX);
    shuffle(10, t);
    for (i = 0; i < 10; i++)
        printf("%d\n", t[i]);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    srand(time(NULL));
    printf("RAND_MAX  = %d", RAND_MAX);
    return 0;
}

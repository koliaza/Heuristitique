#include "util.h"

#include <stdlib.h>
#include <math.h>

int coin_flip(double p) {
    return (rand() <= p*RAND_MAX);
}

/* Note : the quality of random number generation is not that important
   for this project */
int random_int_in_range(int a, int b) {
    return floor( ((double) rand() / (((double) RAND_MAX) + 1.0)) * (a - b + 1) + a );
}

/* Using the Knuth-Fisher-Yates algorithm */
void shuffle(int n, int t[]) {
    int i, j, x;
    for (i = 0 ; i < n ; i++) {
        j = random_int_in_range(i, n-1);
        x = t[i];
        t[i] = t[j];
        t[j] = x;
    }
}

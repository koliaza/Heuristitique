#include "util.h"

#include <stdlib.h>
#include <math.h>


int_list* il_cons(int x, int_list* l) {
    int_list* r = malloc(sizeof(int_list));
    r->x = x;
    r->next = l;
    return r;
}

void il_free(int_list* l) {
    int_list *p, *q;
    p = l;
    while (p != NULL) {
        q = p->next;
        free(p);
        p = q;
    }
}

/* The insert/remove functions might not have great performance
   because they do a lot of branching */

int_list* il_s_insert(int x, int_list* l) {
    int_list *p, *q;

    if (l == NULL || l->x >= x) {
        return il_cons(x, l);
    } else {
        p = l;
        q = l->next;
        while (q != NULL && q->x > x) {
            p = q;
            q = p->next;
        }
        p->next = il_cons(x, q);
        return l;
    }
}

int_list* il_s_insert_once(int x, int_list* l) {
    int_list *p, *q;

    if (l == NULL || l->x > x) {
        return il_cons(x, l);
    } else if (l->x == x)  {
        return l;
    } else {
        p = l;
        q = l->next;
        while (q != NULL && q->x < x) {
            p = q;
            q = p->next;
        }
        p->next = il_cons(x, q);
        return l;
    }    
}

int_list* il_s_remove_once(int x, int_list* l) {
    int_list *p, *prev;
    
    if (l == NULL) {
        return NULL;
    } else if (l->x == x) {
        p = l->next;
        free(l);
        return p;
    } else {
        p = l->next; prev = l;
        while (p != NULL && p->x <= x) {
            if (p->x == x) {
                prev->next = p->next;
                return l;
            }
            prev = p;
            p = p->next;
        }
        return l;
    }
}

int il_s_member(int x, int_list* l) {
    int_list *p;
    for (p = l; p != NULL; p = p->next) {
        if (p->x == x) break;
    }
    return (p != NULL);
}



int coin_flip(double p) {
    return (rand() <= p*RAND_MAX);
}

/* Note : the quality of random number generation is not that important
   for this project */
int random_int_in_range(int a, int b) {
    return floor( ((double) rand() / (((double) RAND_MAX) + 1.0)) * (b - a + 1) + a );
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

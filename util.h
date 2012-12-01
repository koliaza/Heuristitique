#ifndef UTIL_H
#define UTIL_H

/*** Linked lists of integers ***/

struct int_list {
    int x;
    int_list *next;
};
typedef struct int_list int_list;

int_list* il_cons(int x, int_list* l);
/* recursively free all elements */
void il_free(int_list* l);

/* These functions only work correctly on __sorted__ lists,
   and they are __destructive__ */
int_list* il_s_insert(int x, int_list* l);
int_list* il_s_insert_once(int x, int_list* l);
/* int_list* il_s_remove(int x, int_list* l); */
int il_s_member(int x, int_list* l);


/*** Randomness ***/

int coin_flip(double p);
int random_int_in_range(int a, int b);
void shuffle(int n, int t[]);

#endif

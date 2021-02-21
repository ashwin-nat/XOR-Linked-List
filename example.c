#include "xor_ll.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define ITEMS_COUNT         100000

uint64_t get_val (void)
{
    return rand() % (1<<31);
}

int main (int argc, char *argv[])
{
    srand (time(NULL));
    int y=2, z=3;
    int *x = malloc (sizeof(*x));
    int a=10, b=20, c=30;
    *x = 1;
    XOR_LL my_ll = XOR_LL_INITIALISER;
    xor_ll_init (&my_ll);

    xor_ll_push_tail (&my_ll, x, sizeof(*x));
    xor_ll_push_tail (&my_ll, &y, sizeof(y));
    xor_ll_push_tail (&my_ll, &z, sizeof(z));
    xor_ll_push_tail (&my_ll, &a, sizeof(a));
    xor_ll_push_tail (&my_ll, &b, sizeof(b));
    xor_ll_push_head (&my_ll, &c, sizeof(c));

    int i;
    uint64_t *arr = malloc (sizeof(*arr) * ITEMS_COUNT);
    for (i=0; i<ITEMS_COUNT; ++i) {
        arr[i] = get_val ();
    }

    int cat1 = 45, cat2=46;
    XOR_LL_ITERATOR itr = XOR_LL_ITERATOR_INITIALISER;
    printf ("Forward (no changes): ");
    XOR_LL_LOOP_HTT_RST(&my_ll,&itr) {
        int *ptr = itr.node_data.ptr;
        printf ("%d  ", *ptr);
    }

    printf ("\nForward: ");
    XOR_LL_LOOP_HTT_RST(&my_ll,&itr) {
        int *ptr = itr.node_data.ptr;
        // printf ("%d  ", *ptr);
        // fflush(stdout);

        if (*ptr == 3) {
            // xor_ll_remove_node_iter (&my_ll, &itr);
            xor_ll_insert_iter (&my_ll, &itr, &cat1, sizeof(cat1), 
                XOR_LL_INSERTION_BEFORE_ITER);
        }

        if (*ptr == 2) {
            xor_ll_insert_iter (&my_ll, &itr, &cat2, sizeof(cat2), 
                XOR_LL_INSERTION_AFTER_ITER);
        }
    }

    printf ("\nForward (after changes): ");
    XOR_LL_LOOP_HTT_RST(&my_ll,&itr) {
        int *ptr = itr.node_data.ptr;
        printf ("%d  ", *ptr);
    }

    for (i=0; i<ITEMS_COUNT; ++i) {
        xor_ll_push_tail (&my_ll, &arr[i], sizeof(*arr));
    }

    printf ("\n");
    xor_ll_destroy (&my_ll);
    free (x);
    free (arr);
    return 0;
}

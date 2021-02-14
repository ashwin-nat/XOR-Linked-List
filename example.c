#include "xor_ll.h"
#include <stdio.h>
#include <stdlib.h>

int comparator (const void *a, size_t a_sz, const void *b, size_t b_sz)
{
    if (a_sz == b_sz) {
        const int *a_int = a;
        const int *b_int = b;
        if (*a_int == *b_int) {
            return 0;
        }
    }
    return 1;
}

int main (int argc, char *argv[])
{
    printf ("Hello world\n");

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
#if 0

    XOR_LL_LOOP (&my_ll) {
        printf ("loop\n");
    }

    xor_ll_destroy (&xor_ll);
#endif

    XOR_LL_ITERATOR itr = XOR_LL_ITERATOR_INITIALISER;
    printf ("Forward: ");
    XOR_LL_LOOP_HTT_RST(&my_ll,&itr) {
        int *ptr = itr.node_data.ptr;
        printf ("%d  ", *ptr);
        fflush(stdout);

        if (*ptr == 10) {
            xor_ll_remove_node_iter (&my_ll, &itr);
        }
    }

    printf ("\nReverse: ");
    XOR_LL_LOOP_TTH_RST(&my_ll,&itr) {
        int *ptr = itr.node_data.ptr;
        printf ("%d  ", *ptr);
    }

    printf ("\n");
    free (x);
    xor_ll_destroy (&my_ll);
    return 0;
}

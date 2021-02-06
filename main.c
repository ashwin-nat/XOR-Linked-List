#include "xor_ll.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    printf ("Hello world\n");

    int y=2, z=3;
    int *x = malloc (sizeof(*x));
    *x = 1;
    XOR_LL my_ll = XOR_LL_INITIALISER;
    xor_ll_init (&my_ll);

    xor_ll_insert (&my_ll, x, sizeof(*x));
    xor_ll_insert (&my_ll, &y, sizeof(y));
    xor_ll_insert (&my_ll, &z, sizeof(z));
#if 0

    XOR_LL_LOOP (&my_ll) {
        printf ("loop\n");
    }

    xor_ll_destroy (&xor_ll);
#endif

    XOR_LL_ITERATOR itr = XOR_LL_ITERATOR_INITIALISER;
    while (xor_ll_iterate_fwd(&my_ll, &itr) == XOR_LL_STATUS_SUCCESS){
        int *ptr = itr.data_ptr;
        printf ("%d\n", *ptr);
    }

    free (x);
    return 0;
}
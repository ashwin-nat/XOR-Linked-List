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
    printf ("Forward: ");
    XOR_LL_LOOP_FWD(&my_ll,&itr) {
        int *ptr = itr.data_ptr;
        printf ("%d  ", *ptr);
        fflush(stdout);
    }

    printf ("\nReverse: ");
    XOR_LL_LOOP_REV_RST(&my_ll,&itr) {
        int *ptr = itr.data_ptr;
        printf ("%d  ", *ptr);
    }

    printf ("\n");
    free (x);
    xor_ll_destroy (&my_ll);
    return 0;
}
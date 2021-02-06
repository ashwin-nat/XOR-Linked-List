#include "xor_ll.h"
#include <stdio.h>

int main (int argc, char *argv[])
{
    printf ("Hello world\n");
#if 0
    int y=2, z=3;
    int *x = malloc (sizeof(*x));
    *x = 1;
    XOR_LL my_ll;
    xor_ll_init (&my_ll);

    xor_ll_insert (&my_ll, x, sizeof(*x), XOR_LL_DONT_COPY);
    xor_ll_insert (&my_ll, &y, sizeof(y), XOR_LL_COPY_ONTO_HEAP);
    xor_ll_insert (&my_ll, &z, sizeof(z), XOR_LL_DONT_COPY);

    XOR_LL_LOOP (&my_ll) {
        printf ("loop\n");
    }

    xor_ll_destroy (&xor_ll);
#endif
    return 0;
}
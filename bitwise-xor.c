#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

void xor16 (char *num1_str, char *num2_str)
{
    uint16_t num1 = (uint16_t) strtoul (num1_str, NULL, 0);
    uint16_t num2 = (uint16_t) strtoul (num2_str, NULL, 0);

    printf ("0x%04" PRIX16 "\n", num1 ^ num2);
}

void xor32 (char *num1_str, char *num2_str)
{
    uint32_t num1 = (uint32_t) strtoul (num1_str, NULL, 0);
    uint32_t num2 = (uint32_t) strtoul (num2_str, NULL, 0);

    printf ("0x%08" PRIX32 "\n", num1 ^ num2);
}

void xor64 (char *num1_str, char *num2_str)
{
    uint64_t num1 = (uint64_t) strtoull (num1_str, NULL, 0);
    uint64_t num2 = (uint64_t) strtoull (num2_str, NULL, 0);

    printf ("0x%016" PRIX64 "\n", num1 ^ num2);
}

int main (int argc, char *argv[])
{
    if (argc != 3) {
        printf ("usage: %s <num_1> <num_2>\n", argv[0]);
        return 1;
    }

    switch (sizeof(uintptr_t)) {
    case 2:
        xor16 (argv[1], argv[2]);
        break;
    case 4:
        xor32 (argv[1], argv[2]);
        break;
    case 8:
        xor64 (argv[1], argv[2]);
        break;
    default:
        printf ("%ld bit architecture not supported\n", sizeof(uintptr_t)*8);
        break;
    }

    return 0;
}
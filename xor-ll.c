#include "xor_ll.h"
#include <stdlib.h>
#include <stdint.h>
/******************************************************************************/
#define __XOR_LL_TRUE               1
#define __XOR_LL_FALSE              0
#define __XOR_LL_SUCCESS            0
#define __XOR_LL_FAILURE            -1
/******************************************************************************/
struct _xor_ll_node {
    size_t size;
    void *data;
    uintptr_t xor_ptr;
};
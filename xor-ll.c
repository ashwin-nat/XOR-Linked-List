#include "xor_ll.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
/******************************************************************************/
/* File local macros */
#define __XOR_LL_TRUE               1
#define __XOR_LL_FALSE              0
#define __XOR_LL_NULL_PTR           (uintptr_t)(NULL)

//evaluates to 1 if valid, 0 if invalid
#define __XOR_LL_CP_MODE_VALID(x)   ((XOR_LL_DONT_ALLOC==(x)) || \
                                     (XOR_LL_ALLOC_COPY_ONTO_HEAP==(x)))
/******************************************************************************/
/* Internal structure definitions */
/**
 * @brief           The structure used for representing list nodes
 */
struct _xor_ll_node {
    size_t size;
    void *data;
    uintptr_t xor_ptr;
};
/******************************************************************************/
/* Static function declarations */
/**
 * @brief           Creates an XOR Linked list node and returns the pointer
 * @param data      Pointer to the data
 * @param size      Size of the data
 * @return struct _xor_ll_node* - node pointer if success or NULL if failure
 */
static
struct _xor_ll_node*
_xor_ll_create_node (
    const void *data, 
    size_t size);

/**
 * @brief           Computes the next node to iterate to
 * @param prev      Previous node pointer
 * @param xor_ptr   The XOR pointer value
 * @return struct _xor_ll_node* - Next node address
 */
static inline
struct _xor_ll_node*
_xor_ll_get_next_node (
    struct _xor_ll_node *prev,
    uintptr_t xor_ptr);
/******************************************************************************/
/* Public function definitions */
/**
 * @brief           Initialise the XOR Linked list object's fields
 * @param ll_ptr    Pointer to the XOR linked list object
 * @return int      XOR_LL_STATUS_SUCCESS on success
 *                  XOR_LL_STATUS_NULL_XOR_LL if ll_ptr is NULL
 */
int
xor_ll_init (
    XOR_LL *ll_ptr)
{
    if (NULL == ll_ptr) {
        return XOR_LL_STATUS_NULL_XOR_LL;
    }
    ll_ptr->head = NULL;
    ll_ptr->tail = NULL;
    return XOR_LL_STATUS_SUCCESS;
}

/**
 * @brief           Insert the specified data into the XOR linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param data      Const pointer to the data
 * @param size      Size of the data
 * @return int      XOR_LL_STATUS_SUCCESS - insertion successful
 *                  XOR_LL_STATUS_FAILURE_ALLOC - allocation failure (
 *                      if copy_mode is XOR_LL_ALLOC_COPY_ONTO_HEAP)
 *                  XOR_LL_STATUS_BAD_DATA - data is NULL or size is 0
 */
int
xor_ll_insert (
    XOR_LL *ll_ptr, 
    const void *data, 
    size_t size)
{
    assert (ll_ptr);
    //1. check the inputs
    if ((NULL == data) || (0 == size)) {
        return XOR_LL_STATUS_BAD_DATA;
    }

    //2. create the node
    struct _xor_ll_node *node = _xor_ll_create_node (data, size);
    if (!node) {
        return XOR_LL_STATUS_FAILURE_ALLOC;
    }

    //3. establish the links
    if (NULL == ll_ptr->head) {
        //if this is the first node
        assert (NULL == ll_ptr->tail);
        ll_ptr->head = (node);
        ll_ptr->tail = (node);
    }
    else {
        //insert from the tail side
        //1. update xor_ptr to prev (current tail ptr) XOR'd with new (next)node
        ll_ptr->tail->xor_ptr ^= (uintptr_t)(node);

        //2. update the xor_ptr of the new node
        node->xor_ptr = (uintptr_t) (ll_ptr->tail);

        //3. update the tail pointer
        ll_ptr->tail = node;
        
    }
    return XOR_LL_STATUS_SUCCESS;
}

/**
 * @brief           Iterates over the given XOR Linked list in the forward 
 *                      direction, sets data_ptr to point to the required data, 
 *                      and fills the memory pointed to size_ptr (if not NULL) 
 *                      with the data size
 * @param ll_ptr    Pointer to the XOR linked list object
 * @param itr_ptr   Pointer to the XOR LL iterator where the data 
 *                      pointer and size will be updated
 * @return int      XOR_LL_STATUS_SUCCESS - next node found
 *                  XOR_LL_STATUS_EOL - the end of the list is reached
 */
int
xor_ll_iterate_fwd (
    XOR_LL *ll_ptr,
    XOR_LL_ITERATOR *itr_ptr)
{
    assert (ll_ptr);
    //emtpy list
    if (NULL == ll_ptr->head) {
        assert (NULL == ll_ptr->tail);
        return XOR_LL_STATUS_EOL;
    }

    //get the next pointer
    if (NULL == itr_ptr->iterator_curr) {
        assert (NULL == itr_ptr->iterator_prev);
        //there is no current traversal ongoing (prev is NULL in this case)
        itr_ptr->iterator_curr = ll_ptr->head;
    }
    else {
        //proceed with the current traversal, find the next node
        //next node is prev XOR'd with xor_ptr
        struct _xor_ll_node *curr_node = itr_ptr->iterator_curr;
        itr_ptr->iterator_curr = _xor_ll_get_next_node (itr_ptr->iterator_prev, 
                                            itr_ptr->iterator_curr->xor_ptr);
        itr_ptr->iterator_prev = curr_node;
    }

    //if there is a next node, update the return pointer
    if (itr_ptr->iterator_curr) {
        //the node was found, copy the data
        itr_ptr->data_ptr   = itr_ptr->iterator_curr->data;
        itr_ptr->size       = itr_ptr->iterator_curr->size;
        return XOR_LL_STATUS_SUCCESS;
    }
    else {
        //end of list reached, reset iterator pointers
        itr_ptr->iterator_prev = NULL;
        return XOR_LL_STATUS_EOL;
    }
}

/**
 * @brief           Iterates over the given XOR Linked list in the reverse 
 *                      direction, sets data_ptr to point to the required data, 
 *                      and fills the memory pointed to size_ptr (if not NULL) 
 *                      with the data size
 * @param ll_ptr    Pointer to the XOR linked list object
 * @param itr_ptr   Pointer to the XOR LL iterator where the data 
 *                      pointer and size will be updated
 * @return int      XOR_LL_STATUS_SUCCESS - next node found
 *                  XOR_LL_STATUS_EOL - the end of the list is reached
 */
int
xor_ll_iterate_rev (
    XOR_LL *ll_ptr,
    XOR_LL_ITERATOR *itr_ptr)
{
    assert (ll_ptr);
    //emtpy list
    if (NULL == ll_ptr->tail) {
        assert (NULL == ll_ptr->head);
        return XOR_LL_STATUS_EOL;
    }

    //get the next pointer
    if (NULL == itr_ptr->iterator_curr) {
        assert (NULL == itr_ptr->iterator_prev);
        //there is no current traversal ongoing (prev is NULL in this case)
        itr_ptr->iterator_curr = ll_ptr->tail;
    }
    else {
        //proceed with the current traversal, find the next node
        //next node is prev XOR'd with xor_ptr
        struct _xor_ll_node *curr_node = itr_ptr->iterator_curr;
        itr_ptr->iterator_curr = _xor_ll_get_next_node (itr_ptr->iterator_prev, 
                                            itr_ptr->iterator_curr->xor_ptr);
        itr_ptr->iterator_prev = curr_node;
    }

    //if there is a next node, update the return pointer
    if (itr_ptr->iterator_curr) {
        //the node was found, copy the data
        itr_ptr->data_ptr   = itr_ptr->iterator_curr->data;
        itr_ptr->size       = itr_ptr->iterator_curr->size;
        return XOR_LL_STATUS_SUCCESS;
    }
    else {
        //end of list reached, reset iterator pointers
        itr_ptr->iterator_prev = NULL;
        return XOR_LL_STATUS_EOL;
    }
}

/**
 * @brief           End/Reset the ongoing iteration prematurely
 * @param itr_ptr   Pointer to the XOR Linked list iterator object
 */
void
xor_ll_reset_iteration (
    XOR_LL_ITERATOR *itr_ptr)
{
    itr_ptr->data_ptr       = NULL;
    itr_ptr->size           = 0;
    itr_ptr->iterator_curr  = NULL;
    itr_ptr->iterator_prev  = NULL;
}

/**
 * @brief           Destroy this XOR Linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @return int      XOR_LL_STATUS_SUCCESS Linked list destroyed successfully
 */
int
xor_ll_destroy (
    XOR_LL *ll_ptr)
{
    assert (ll_ptr);
    //iterate through the linked list, and free all the memory allocated
    struct _xor_ll_node *curr = ll_ptr->head, *prev = NULL;
    while (curr) {
        //update the next and prev pointers
        struct _xor_ll_node *temp = curr;
        curr = _xor_ll_get_next_node (prev, curr->xor_ptr);
        prev = temp;

        //free prev and it's data
        free (prev->data);
        free (prev);
    }

    //reset the head and tail pointers
    ll_ptr->head = NULL;
    ll_ptr->tail = NULL;

    return XOR_LL_STATUS_SUCCESS;
}

/******************************************************************************/
/* Static function definitions */
/**
 * @brief           Creates an XOR Linked list node and returns the pointer
 * @param data      Pointer to the data
 * @param size      Size of the data
 * @return struct _xor_ll_node* - node pointer if success or NULL if failure
 */
static
struct _xor_ll_node*
_xor_ll_create_node (
    const void *data, 
    size_t size)
{
    //allocate memory for the node
    struct _xor_ll_node *ret = malloc (sizeof(*ret));
    if (ret) {
        //allocate memory for the data
        ret->data = malloc (size);
        if (ret->data) {
            //if data allocation is successful, copy the data;
            memcpy (ret->data, data, size);
            ret->size = size;
            ret->xor_ptr = __XOR_LL_NULL_PTR;
        }
        //if data allocation failed, free the node allocation and return NULL
        else {
            free (ret);
            ret = NULL;
        }
    }
    return ret;
}

/**
 * @brief           Computes the next node to iterate to
 * @param prev      Previous node pointer
 * @param xor_ptr   The XOR pointer value
 * @return struct _xor_ll_node* - Next node address
 */
static inline
struct _xor_ll_node*
_xor_ll_get_next_node (
    struct _xor_ll_node *prev,
    uintptr_t xor_ptr)
{
    return (struct _xor_ll_node*) ((uintptr_t)(prev) ^ xor_ptr);
}
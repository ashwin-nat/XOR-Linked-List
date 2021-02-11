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
#define __XOR_LL_COMPUTE_LINK(a,b)  (((uintptr_t)(a)) ^ ((uintptr_t)(b)))
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
 */
int
xor_ll_init (
    XOR_LL *ll_ptr)
{
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
        // ll_ptr->tail->xor_ptr ^= (uintptr_t)(node);
        ll_ptr->tail->xor_ptr = __XOR_LL_COMPUTE_LINK(ll_ptr->tail->xor_ptr,
                                    node);

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

    //just deleted - update data ptr
    if (__XOR_LL_TRUE == itr_ptr->just_deleted) {
        //if itr_ptr->iterator_curr is NULL, we have reached the end of the list
        itr_ptr->just_deleted = __XOR_LL_FALSE;
        if (itr_ptr->iterator_curr) {
            itr_ptr->data_ptr = itr_ptr->iterator_curr->data;
            itr_ptr->size = itr_ptr->iterator_curr->size;
        }
        else {
            itr_ptr->data_ptr = NULL;
            itr_ptr->size = 0;
        }
        return (itr_ptr->iterator_curr) ? (XOR_LL_STATUS_SUCCESS) : 
            (XOR_LL_STATUS_EOL);
    }

    itr_ptr->forward_dir = __XOR_LL_TRUE;
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
        itr_ptr->iterator_curr = _xor_ll_get_next_node (
                                    itr_ptr->iterator_prev, 
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

    //just deleted
    if (__XOR_LL_TRUE == itr_ptr->just_deleted) {
        itr_ptr->just_deleted = __XOR_LL_FALSE;
        //if itr_ptr->iterator_curr is NULL, we have reached the end of the list
        if (itr_ptr->iterator_curr) {
            itr_ptr->data_ptr = itr_ptr->iterator_curr->data;
            itr_ptr->size = itr_ptr->iterator_curr->size;
        }
        else {
            itr_ptr->data_ptr = NULL;
            itr_ptr->size = 0;
        }
        return (itr_ptr->iterator_curr) ? (XOR_LL_STATUS_SUCCESS) : 
            (XOR_LL_STATUS_EOL);
    }

    itr_ptr->forward_dir = __XOR_LL_FALSE;
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
xor_ll_reset_iterator (
    XOR_LL_ITERATOR *itr_ptr)
{
    itr_ptr->data_ptr       = NULL;
    itr_ptr->size           = 0;
    itr_ptr->iterator_curr  = NULL;
    itr_ptr->iterator_prev  = NULL;
    itr_ptr->forward_dir    = __XOR_LL_TRUE;
    itr_ptr->just_deleted   = __XOR_LL_FALSE;
}

/**
 * @brief           Remove the node specified by the iterator from the linked 
 *                  list
 * @param ll_ptr    Pointer to the XOR linked list object
 * @param iter_ptr  Pointer to the iterator describing the position
 * @return int      XOR_LL_STATUS_SUCCESS removal successful
 *                  XOR_LL_STATUS_EMPTY_LIST list is empty
 */
int
xor_ll_remove_node_iter (
    XOR_LL *ll_ptr,
    XOR_LL_ITERATOR *iter_ptr)
{
    struct _xor_ll_node *del_ptr = iter_ptr->iterator_curr;

    /* NOTE:    The terms prev and next in this function are based on the 
                assumption that we are traversing forward */

    //case 0: list is empty
    if (!ll_ptr->head && !ll_ptr->tail) {

        assert (iter_ptr->iterator_curr == NULL);
        assert (iter_ptr->iterator_prev == NULL);
        return XOR_LL_STATUS_EMPTY_LIST;
    }

    //case 1: list has only one node, and that is to be removed
    else if ((ll_ptr->head == ll_ptr->tail) && 
            (iter_ptr->iterator_curr == ll_ptr->head)) {

        assert (iter_ptr->iterator_prev == NULL);
        //remove that node, and update tail, head, and iterator to NULL
        ll_ptr->head = NULL;
        ll_ptr->tail = NULL;
        iter_ptr->iterator_curr = NULL;
    }

    //case 2: list has multiple nodes, node to be removed is head
    else if (iter_ptr->iterator_curr == ll_ptr->head) {

        //find the next and next->next addresses
        struct _xor_ll_node *next = _xor_ll_get_next_node (NULL, 
                                        iter_ptr->iterator_curr->xor_ptr);
        struct _xor_ll_node *next_next = _xor_ll_get_next_node (ll_ptr->head, 
                                            next->xor_ptr);

        //update link of next. link from NULL to next->next
        //next->xor_ptr = (NULL) XOR &(next->next)
        next->xor_ptr = __XOR_LL_COMPUTE_LINK(NULL,next_next);

        //update the head pointer and the iterator
        ll_ptr->head = next;
        if (__XOR_LL_TRUE == iter_ptr->forward_dir) {
            iter_ptr->iterator_curr = next;
        }
        else {
            iter_ptr->iterator_curr = NULL;
        }
    }

    //case 3: list has multiple nodes, node to be removed is tail
    else if (iter_ptr->iterator_curr == ll_ptr->tail) {

        //find the prev and prev->prev addresses
        struct _xor_ll_node *prev = _xor_ll_get_next_node (NULL, 
                                        iter_ptr->iterator_curr->xor_ptr);
        struct _xor_ll_node *prev_prev = _xor_ll_get_next_node (ll_ptr->tail, 
                                            prev->xor_ptr);

        //update link of prev. link from NULL to prev->prev
        //prev->xor_ptr = (NULL) XOR &(prev->prev)
        prev->xor_ptr = __XOR_LL_COMPUTE_LINK(NULL,prev_prev);

        //update the tail pointer and iterator
        ll_ptr->tail = prev;
        if (__XOR_LL_TRUE == iter_ptr->forward_dir) {
            iter_ptr->iterator_curr = NULL;
        }
        else {
            iter_ptr->iterator_curr = prev;
        }
    }

    //case 4: list has multiple nodes, our node is somewhere in between
    else {

        //find the next, next->next and prev, and prev->prev node addresses
        struct _xor_ll_node *next = _xor_ll_get_next_node (
                                        iter_ptr->iterator_prev,
                                        iter_ptr->iterator_curr->xor_ptr);
        struct _xor_ll_node *next_next = _xor_ll_get_next_node (
                                        iter_ptr->iterator_curr,
                                        next->xor_ptr);
        struct _xor_ll_node *prev = iter_ptr->iterator_prev;
        struct _xor_ll_node *prev_prev = _xor_ll_get_next_node (
                                        iter_ptr->iterator_curr,
                                        prev->xor_ptr);

        //update link of prev. link from prev->prev to next
        //prev->xor_ptr = &(prev->prev) XOR &(next)
        prev->xor_ptr = __XOR_LL_COMPUTE_LINK(prev_prev, next);

        //update link of next. link from next->next to prev
        next->xor_ptr = __XOR_LL_COMPUTE_LINK(next_next, prev);

        //update iterator (direction doesn't matter here)
        iter_ptr->iterator_curr = next;

    }

    //cleanup section
    if (del_ptr) {
#ifndef XOR_LL_NO_ALLOC
        free (del_ptr->data);
#endif
        free (del_ptr);
        iter_ptr->just_deleted = __XOR_LL_TRUE;
        iter_ptr->data_ptr = NULL;
        iter_ptr->size = 0;
    }
    return XOR_LL_STATUS_SUCCESS;
}

/**
 * @brief           Search for the given search key in the given linked list and
 *                      remove it
 * @param ll_ptr    Pointer to the XOR Linked List object
 * @param key       Const void pointer to the search key data
 * @param size      Size of the search key
 * @param cmp       Comparator function that returns 0 when the search key is 
 *                      found, and non-zero otherwise
 * @return int      XOR_LL_STATUS_SUCCESS removal successful
 *                  XOR_LL_STATUS_NOT_FOUND key not found
 */
int
xor_ll_remove_node (
    XOR_LL *ll_ptr,
    const void *key,
    size_t size,
    int (*cmp) (const void *a, size_t a_sz, const void *b, size_t b_sz))
{
    assert (ll_ptr);
    assert (key);
    assert (size);

    //iterate through the linked list, and free all the memory allocated
    struct _xor_ll_node *curr = ll_ptr->head, *prev = NULL;
    while (curr) {
        //apply the comparator function for all items
        if (cmp(curr->data, curr->size, key, size) == 0) {
            //found the key;
            if (curr == ll_ptr->head && curr == ll_ptr->tail) {
                //only one item in the list, we're removing it
                ll_ptr->head = NULL;
                ll_ptr->tail = NULL;
            }
            else if (curr == ll_ptr->head) {
                //delete the head node
                //find the 3rd node in the list
                struct _xor_ll_node *next = (struct _xor_ll_node*) 
                                            (ll_ptr->head->xor_ptr);
                struct _xor_ll_node *next_next = _xor_ll_get_next_node (
                                            curr, next->xor_ptr);
                //shift the head one step right
                ll_ptr->head = next;
                next->xor_ptr = (uintptr_t) (next_next);
            }
            else if (curr == ll_ptr->tail) {
                //delete the tail node
                //find the 3rd last node in the list
                //(we know prev is tail->xor_ptr)
                struct _xor_ll_node *prev_prev = _xor_ll_get_next_node
                                            (curr, prev->xor_ptr);
                //shift the tail one step left
                ll_ptr->tail = prev;
                prev->xor_ptr = (uintptr_t) (prev_prev);
            }
            else {
                //the node is somewhere in between
                //update the xor_ptr of prev and next so that curr gets unlinked
                struct _xor_ll_node *next = _xor_ll_get_next_node (prev, 
                                                curr->xor_ptr);
                struct _xor_ll_node *prev_prev = _xor_ll_get_next_node (curr, 
                                                prev->xor_ptr);
                struct _xor_ll_node *next_next = _xor_ll_get_next_node (curr, 
                                                next->xor_ptr);
                //set the new xor_ptr for prev and next
                // prev->xor_ptr = (uintptr_t)(prev_prev) ^ (uintptr_t)(next);
                // next->xor_ptr = (uintptr_t)(prev) ^ (uintptr_t)(next_next);
                prev->xor_ptr = __XOR_LL_COMPUTE_LINK(prev_prev, next);
                next->xor_ptr = __XOR_LL_COMPUTE_LINK(prev, next_next);
            }

#ifndef XOR_LL_NO_ALLOC
            free (curr->data);
#endif
            free (curr);

            return XOR_LL_STATUS_SUCCESS;
        }

        //update the next and prev pointers
        struct _xor_ll_node *temp = curr;
        curr = _xor_ll_get_next_node (prev, curr->xor_ptr);
        prev = temp;
    }
    return XOR_LL_STATUS_NOT_FOUND;
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
#ifndef XOR_LL_NO_ALLOC
        free (prev->data);
#endif
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
#ifdef XOR_LL_NO_ALLOC
        ret->data = data;
        ret->size = size;
#else
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
#endif
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
    // return (struct _xor_ll_node*) ((uintptr_t)(prev) ^ xor_ptr);
    return (struct _xor_ll_node*) (__XOR_LL_COMPUTE_LINK(prev,xor_ptr));
}
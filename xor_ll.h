/**
 * @file            xor_ll.h
 * @author          Ashwin Natarajan
 * @brief           The header file for the XOR linked list library. This 
 *                  should be added to the include directory of your project
 * @copyright       Copyright (c) 2021
 * 
 */
#ifndef __XOR_LL_H__
#define __XOR_LL_H__

#include <stdint.h>
#include <stddef.h>

/******************************************************************************/
/* Macro definitions */
#define XOR_LL_STATUS_SUCCESS            0
#define XOR_LL_STATUS_FAILURE_GEN       -1
#define XOR_LL_STATUS_FAILURE_ALLOC     -2
#define XOR_LL_STATUS_BAD_DATA          -3
#define XOR_LL_STATUS_EOL               -4
#define XOR_LL_STATUS_NOT_FOUND         -5
#define XOR_LL_STATUS_EMPTY_LIST        -6

#define XOR_LL_INITIALISER              {   .head = NULL, \
                                            .tail = NULL,}
#define XOR_LL_ITERATOR_INITIALISER     {   .data_ptr = NULL, .size = 0, \
                                            .iterator_prev = NULL, \
                                            .iterator_curr = NULL, \
                                            .forward_dir = 1, \
                                            .just_deleted = 0,}

#define XOR_LL_DONT_ALLOC               0
#define XOR_LL_ALLOC_COPY_ONTO_HEAP     1

#define XOR_LL_LOOP_FWD(ll_ptr,ll_itr_ptr) \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_fwd((ll_ptr),(ll_itr_ptr)))
#define XOR_LL_LOOP_FWD_RST(ll_ptr,ll_itr_ptr) \
                            xor_ll_reset_iterator((ll_itr_ptr)); \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_fwd((ll_ptr),(ll_itr_ptr)))

#define XOR_LL_LOOP_REV(ll_ptr,ll_itr_ptr) \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_rev((ll_ptr),(ll_itr_ptr)))
#define XOR_LL_LOOP_REV_RST(ll_ptr,ll_itr_ptr) \
                            xor_ll_reset_iterator((ll_itr_ptr)); \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_rev((ll_ptr),(ll_itr_ptr)))
/******************************************************************************/
/* Strucure definitions */
/**
 * @brief           The opaque data structure used to represent a node.
 *                  Just the forward declaration here
 */
struct _xor_ll_node;

/**
 * @brief           The structure for the XOR linked list handle
 */
typedef struct _xor_ll {
    struct _xor_ll_node *head;
    struct _xor_ll_node *tail;
}XOR_LL;

typedef struct _xor_ll_iterator {
    void *data_ptr;
    size_t size;

    struct _xor_ll_node *iterator_prev;
    struct _xor_ll_node *iterator_curr;

    uint8_t forward_dir;
    uint8_t just_deleted;
}XOR_LL_ITERATOR;

/******************************************************************************/
/* Function declarations */
/**
 * @brief           Initialise the XOR Linked list object's fields
 * @param ll_ptr    Pointer to the XOR linked list object
 * @return int      XOR_LL_STATUS_SUCCESS on success (will never fail)
 */
int
xor_ll_init (
    XOR_LL *ll_ptr);

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
    size_t size);

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
    XOR_LL_ITERATOR *itr_ptr);

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
    XOR_LL_ITERATOR *itr_ptr);

/**
 * @brief           End/Reset the ongoing iteration prematurely
 * @param itr_ptr   Pointer to the XOR Linked list iterator object
 */
void
xor_ll_reset_iterator (
    XOR_LL_ITERATOR *itr_ptr);

/**
 * @brief           Remove the node specified by the iterator from the linked 
 *                  list
 * @param ll_ptr    Pointer to the XOR linked list object
 * @param iter_ptr  Pointer to the iterator describing the position
 * @return int      XOR_LL_STATUS_SUCCESS - removal successful
 *                  XOR_LL_STATUS_EMPTY_LIST - empty list
 */
int
xor_ll_remove_node_iter (
    XOR_LL *ll_ptr,
    XOR_LL_ITERATOR *iter_ptr);
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
    int (*cmp) (const void *a, size_t a_sz, const void *b, size_t b_sz));

/**
 * @brief           Destroy this XOR Linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @return int      XOR_LL_STATUS_SUCCESS Linked list destroyed successfully
 */
int
xor_ll_destroy (
    XOR_LL *ll_ptr);

#endif  //__XOR_LL_H__
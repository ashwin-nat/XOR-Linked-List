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
//Status codes
#define XOR_LL_STATUS_SUCCESS            0
#define XOR_LL_STATUS_FAILURE_GEN       -1
#define XOR_LL_STATUS_FAILURE_ALLOC     -2
#define XOR_LL_STATUS_BAD_OPTIONS       -3
#define XOR_LL_STATUS_EOL               -4
#define XOR_LL_STATUS_NOT_FOUND         -5
#define XOR_LL_STATUS_EMPTY_LIST        -6

//options
#define XOR_LL_INSERTION_BEFORE_ITER    0
#define XOR_LL_INSERTION_AFTER_ITER     1
//convenience macros
/**
 * @brief   Assign this to your stack allocated XOR_LL object to initialise it
 * 
 */
#define XOR_LL_INITIALISER              {   .head = NULL, \
                                            .tail = NULL,}
/**
 * @brief   Assign this to your stack allocated XOR_LL_ITERATOR to init it
 * 
 */
#define XOR_LL_ITERATOR_INITIALISER     {   .node_data.ptr = NULL, \
                                            .node_data.size = 0, \
                                            .iterator_prev = NULL, \
                                            .iterator_curr = NULL, \
                                            .htt_dir = 1, \
                                            .just_deleted = 0,}

/**
 * @brief   Convenience macro for traversing the Loop in Head to Tail direction
 *          Does NOT initialise/reset the iterator
 * 
 */
#define XOR_LL_LOOP_HTT(ll_ptr,ll_itr_ptr) \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_htt((ll_ptr),(ll_itr_ptr)))
/**
 * @brief   Convenience macro for traversing the Loop in Head to Tail direction
 *          Also resets/initilises the iterator at the start of the loop
 * 
 */
#define XOR_LL_LOOP_HTT_RST(ll_ptr,ll_itr_ptr) \
                            xor_ll_reset_iterator((ll_itr_ptr)); \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_htt((ll_ptr),(ll_itr_ptr)))

/**
 * @brief   Convenience macro for traversing the Loop in Tail to Head direction
 *          Does NOT initialise/reset the iterator
 * 
 */
#define XOR_LL_LOOP_TTH(ll_ptr,ll_itr_ptr) \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_tth((ll_ptr),(ll_itr_ptr)))
/**
 * @brief   Convenience macro for traversing the Loop in Tail to Head direction
 *          Also resets/initilises the iterator at the start of the loop
 * 
 */
#define XOR_LL_LOOP_TTH_RST(ll_ptr,ll_itr_ptr) \
                            xor_ll_reset_iterator((ll_itr_ptr)); \
                            while(XOR_LL_STATUS_EOL != \
                            xor_ll_iterate_tth((ll_ptr),(ll_itr_ptr)))
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

/**
 * @brief           The structure for returning the data and size to the caller
 * 
 */
typedef struct _xor_ll_node_data {
    void *ptr;
    size_t size;
}XOR_LL_NODE_DATA;

/**
 * @brief           The structure for maintaining list traversal state data
 * 
 */
typedef struct _xor_ll_iterator {
    XOR_LL_NODE_DATA node_data;

    struct _xor_ll_node *iterator_prev;
    struct _xor_ll_node *iterator_curr;

    uint8_t htt_dir;
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
 * @brief           Push the specified data into the XOR linked list from the 
 *                  tail side
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param data      Const pointer to the data
 * @param size      Size of the data
 * @return int      XOR_LL_STATUS_SUCCESS - insertion successful
 *                  XOR_LL_STATUS_FAILURE_ALLOC - allocation failure
 */
int
xor_ll_push_tail (
    XOR_LL *ll_ptr, 
    const void *data, 
    size_t size);

/**
 * @brief           Push the specified data into the XOR linked list from the 
 *                  head side
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param data      Const pointer to the data
 * @param size      Size of the data
 * @return int      XOR_LL_STATUS_SUCCESS - insertion successful
 *                  XOR_LL_STATUS_FAILURE_ALLOC - allocation failure 
 */
int
xor_ll_push_head (
    XOR_LL *ll_ptr, 
    const void *data, 
    size_t size);

/**
 * @brief           Insert the given data at the position specified by the 
 *                  iterator
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param itr_ptr   Pointer to the iterator object
 * @param data      Pointer to the data
 * @param size      Size of the data
 * @param position  XOR_LL_INSERTION_BEFORE_ITER - insert before iterator
 *                  XOR_LL_INSERTION_AFTER_ITER  - insert after iterator
 * @return int      XOR_LL_STATUS_SUCCESS - insertion successful
 *                  XOR_LL_STATUS_FAILURE_ALLOC - allocation failure
 */
int
xor_ll_insert_iter (
    XOR_LL *ll_ptr,
    XOR_LL_ITERATOR *itr_ptr,
    const void *data,
    size_t size,
    uint8_t position);

/**
 * @brief           Iterates over the given XOR Linked list in the head to tail 
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
xor_ll_iterate_htt (
    XOR_LL *ll_ptr,
    XOR_LL_ITERATOR *itr_ptr);

/**
 * @brief           Iterates over the given XOR Linked list in the tail to head 
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
xor_ll_iterate_tth (
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
 * @brief           Pop the head node from the XOR linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param data      (Optional) Pointer to the node data object (set to NULL if 
 *                  not interested in the data)
 * @return int      XOR_LL_STATUS_SUCCESS - pop successful
 *                  XOR_LL_STATUS_EMPTY_LIST - empty list
 */
int
xor_ll_pop_head (
    XOR_LL *ll_ptr,
    XOR_LL_NODE_DATA *data);

/**
 * @brief           Pop the tail node from the XOR linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param data      (Optional) Pointer to the node data object (set to NULL if 
 *                  not interested in the data)
 * @return int      XOR_LL_STATUS_SUCCESS - pop successful
 *                  XOR_LL_STATUS_EMPTY_LIST - empty list
 */
int
xor_ll_pop_tail (
    XOR_LL *ll_ptr,
    XOR_LL_NODE_DATA *data);

/**
 * @brief           Destroy this XOR Linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @return int      XOR_LL_STATUS_SUCCESS Linked list destroyed successfully
 */
int
xor_ll_destroy (
    XOR_LL *ll_ptr);

#endif  //__XOR_LL_H__
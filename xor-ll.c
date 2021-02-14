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
#define __XOR_LL_INS_MODE_VALID(x)  ((XOR_LL_INSERTION_BEFORE_ITER==(x)) || \
                                     (XOR_LL_INSERTION_AFTER_ITER==(x)))
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

/**
 * @brief           Pops the extreme side node from the linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param data      Pointer to the node data object (set to NULL if not 
 *                  interested)
 * @param is_head_pop __XOR_LL_TRUE - if popping from head side
 *                    __XOR_LL_FALSE - if popping from tail side
 * @return int      XOR_LL_STATUS_SUCCESS - should never fail
 */
static
int
_xor_ll_pop (
    XOR_LL *ll_ptr,
    XOR_LL_NODE_DATA *data,
    uint8_t is_head_pop);
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
    size_t size)
{
    assert (ll_ptr);

    //1. create the node
    struct _xor_ll_node *node = _xor_ll_create_node (data, size);
    if (!node) {
        return XOR_LL_STATUS_FAILURE_ALLOC;
    }

    //2. establish the links
    if (NULL == ll_ptr->head) {
        //if this is the first node
        assert (NULL == ll_ptr->tail);
        ll_ptr->head = (node);
        ll_ptr->tail = (node);
    }
    else {
        //insert from the tail side
        //1. update xor_ptr to prev (current tail ptr) XOR'd with new (next)node
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
    size_t size)
{
    assert (ll_ptr);

    //1. create the node
    struct _xor_ll_node *node = _xor_ll_create_node (data, size);
    if (!node) {
        return XOR_LL_STATUS_FAILURE_ALLOC;
    }

    //2. establish the links
    if (NULL == ll_ptr->head) {
        //if this is the first node
        assert (NULL == ll_ptr->tail);
        ll_ptr->head = (node);
        ll_ptr->tail = (node);
    }
    else {
        //insert from the head side
        //1. update xor_ptr to prev (current head ptr) XOR'd with new (next)node
        ll_ptr->head->xor_ptr = __XOR_LL_COMPUTE_LINK(ll_ptr->head->xor_ptr,
                                    node);

        //2. update the xor_ptr of the new node
        node->xor_ptr = (uintptr_t) (ll_ptr->head);

        //3. update the head pointer
        ll_ptr->head = node;
    }
    return XOR_LL_STATUS_SUCCESS;
}

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
    uint8_t position)
{
    assert (ll_ptr);
    assert (itr_ptr);

    //0. validate options
    if (!__XOR_LL_INS_MODE_VALID(position)) {
        return XOR_LL_STATUS_BAD_OPTIONS;
    }

    //1. create the node
    struct _xor_ll_node *new_node = _xor_ll_create_node (data, size);
    if (NULL == new_node) { return XOR_LL_STATUS_FAILURE_ALLOC; }

    //2. if list empty
    if (NULL == ll_ptr->head) {
        assert (NULL == ll_ptr->tail);
        assert (NULL == itr_ptr->iterator_curr);
        assert (NULL == itr_ptr->iterator_prev);

        ll_ptr->head = new_node;
        ll_ptr->tail = new_node;
        itr_ptr->iterator_curr = new_node;
        return XOR_LL_STATUS_SUCCESS;
    }

    //these variables are redundant, but are made to improve readability
    //the compiler will probably optimise them away anyways
    struct _xor_ll_node *prev = itr_ptr->iterator_prev;
    struct _xor_ll_node *curr = itr_ptr->iterator_curr;
    struct _xor_ll_node *next = _xor_ll_get_next_node (
                                        prev, curr->xor_ptr);

    //3. the "fun" part [NOTE: eol is end of list, sol is start of list]
    if (XOR_LL_INSERTION_BEFORE_ITER == position) {
        /* Insert between prev and curr */
        const uint8_t is_sol = (NULL == prev) ? (__XOR_LL_TRUE) : 
                                (__XOR_LL_FALSE);

        if (__XOR_LL_TRUE == is_sol) {
            //1. compute link for new node. from NULL to curr
            new_node->xor_ptr = __XOR_LL_COMPUTE_LINK(NULL,curr);

            //2. update head's/tail's/prev's link. link from new node to next
            curr->xor_ptr = __XOR_LL_COMPUTE_LINK(new_node,next);

            //3. need to update extremum node
            if (curr == ll_ptr->head) {
                assert (__XOR_LL_TRUE == itr_ptr->htt_dir);

                //insert before head and set that as new head
                //update head pointer to new node
                ll_ptr->head = new_node;
            }
            else {
                assert (curr == ll_ptr->tail);
                assert (__XOR_LL_FALSE == itr_ptr->htt_dir);

                //insert before tail and set that as new head
                //update tail pointer to new node
                ll_ptr->tail = new_node;
            }

            //4. update iterator's previous node to new node
            itr_ptr->iterator_prev = new_node;
        }
        else {
            //0. compute address of prev->prev
            struct _xor_ll_node *prev_prev = _xor_ll_get_next_node (
                                                curr, prev->xor_ptr);
            //1. compute link for new node. link from prev to curr
            new_node->xor_ptr = __XOR_LL_COMPUTE_LINK(prev, curr);

            //2. update link of current. link from new node to next
            curr->xor_ptr = __XOR_LL_COMPUTE_LINK(new_node,next);

            //3. update link of prev. link from prev->prev to new node
            prev->xor_ptr = __XOR_LL_COMPUTE_LINK(prev_prev,new_node);

            //4. update iterator's prev to new node
            itr_ptr->iterator_prev = new_node;
        }
    }
    else {
        /* Insert between curr and next */
        const uint8_t is_eol = (NULL == next) ? (__XOR_LL_TRUE) : 
                                (__XOR_LL_FALSE);

        if (__XOR_LL_TRUE == is_eol) {
            //1. compute link for new node. from curr to NULL
            new_node->xor_ptr = __XOR_LL_COMPUTE_LINK(curr,NULL);

            //2. update head's/tail's/curr's link. link from prev to new node
            curr->xor_ptr = __XOR_LL_COMPUTE_LINK(prev,new_node);

            //3. update extremum node
            if (curr == ll_ptr->tail) {
                assert (__XOR_LL_TRUE == itr_ptr->htt_dir);

                //insert after tail, and set that as new tail
                ll_ptr->tail = new_node;
            }
            else {
                assert (curr == ll_ptr->head);
                assert (__XOR_LL_FALSE == itr_ptr->htt_dir);

                //insert after head (since TTH dir), and set that as new head
                ll_ptr->head = new_node;
            }

            //no need to update iterator's previous, since we're updating after
        }
        else {
            //0. compute address of next->next
            struct _xor_ll_node *next_next = _xor_ll_get_next_node (curr, 
                                                    next->xor_ptr);

            //1. compute link for new node. link from curr to next
            new_node->xor_ptr = __XOR_LL_COMPUTE_LINK(curr,next);

            //2. update link of current. link from prev to new node
            curr->xor_ptr = __XOR_LL_COMPUTE_LINK(prev,new_node);

            //3. update link of next. link from new node to next->next
            next->xor_ptr = __XOR_LL_COMPUTE_LINK(new_node,next_next);
        }
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
xor_ll_iterate_htt (
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
            itr_ptr->node_data.ptr = itr_ptr->iterator_curr->data;
            itr_ptr->node_data.size = itr_ptr->iterator_curr->size;
        }
        else {
            itr_ptr->node_data.ptr = NULL;
            itr_ptr->node_data.size = 0;
        }
        return (itr_ptr->iterator_curr) ? (XOR_LL_STATUS_SUCCESS) : 
            (XOR_LL_STATUS_EOL);
    }

    itr_ptr->htt_dir = __XOR_LL_TRUE;
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
        itr_ptr->node_data.ptr   = itr_ptr->iterator_curr->data;
        itr_ptr->node_data.size       = itr_ptr->iterator_curr->size;
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
xor_ll_iterate_tth (
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
            itr_ptr->node_data.ptr = itr_ptr->iterator_curr->data;
            itr_ptr->node_data.size = itr_ptr->iterator_curr->size;
        }
        else {
            itr_ptr->node_data.ptr = NULL;
            itr_ptr->node_data.size = 0;
        }
        return (itr_ptr->iterator_curr) ? (XOR_LL_STATUS_SUCCESS) : 
            (XOR_LL_STATUS_EOL);
    }

    itr_ptr->htt_dir = __XOR_LL_FALSE;
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
        itr_ptr->node_data.ptr   = itr_ptr->iterator_curr->data;
        itr_ptr->node_data.size       = itr_ptr->iterator_curr->size;
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
    itr_ptr->node_data.ptr  = NULL;
    itr_ptr->node_data.size = 0;
    itr_ptr->iterator_curr  = NULL;
    itr_ptr->iterator_prev  = NULL;
    itr_ptr->htt_dir        = __XOR_LL_TRUE;
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
        if (__XOR_LL_TRUE == iter_ptr->htt_dir) {
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
        if (__XOR_LL_TRUE == iter_ptr->htt_dir) {
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
        free (del_ptr);
        iter_ptr->just_deleted = __XOR_LL_TRUE;
        iter_ptr->node_data.ptr = NULL;
        iter_ptr->node_data.size = 0;
    }
    return XOR_LL_STATUS_SUCCESS;
}

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
    XOR_LL_NODE_DATA *data)
{
    assert (ll_ptr);
    //1. check for empty list
    if (NULL == ll_ptr->head) {
        assert (NULL == ll_ptr->tail);
        return XOR_LL_STATUS_EMPTY_LIST;
    }

    //2. pop node
    return _xor_ll_pop (ll_ptr, data, __XOR_LL_TRUE);
}

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
    XOR_LL_NODE_DATA *data)
{
    assert (ll_ptr);
    //1. check for empty list
    if (NULL == ll_ptr->head) {
        assert (NULL == ll_ptr->tail);
        return XOR_LL_STATUS_EMPTY_LIST;
    }

    //2. pop node
    return _xor_ll_pop (ll_ptr, data, __XOR_LL_FALSE);
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
        ret->data = (void*)data;
        ret->size = size;
        ret->xor_ptr = __XOR_LL_NULL_PTR;
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
    return (struct _xor_ll_node*) (__XOR_LL_COMPUTE_LINK(prev,xor_ptr));
}

/**
 * @brief           Pops the extreme side node from the linked list
 * @param ll_ptr    Pointer to the XOR Linked list object
 * @param data      Pointer to the node data object (set to NULL if not 
 *                  interested)
 * @param is_head_pop __XOR_LL_TRUE - if popping from head side
 *                    __XOR_LL_FALSE - if popping from tail side
 * @return int      XOR_LL_STATUS_SUCCESS - should never fail
 */
static
int
_xor_ll_pop (
    XOR_LL *ll_ptr,
    XOR_LL_NODE_DATA *data,
    uint8_t is_head_pop)
{
    //1. create and initialise iterator
    XOR_LL_ITERATOR iter = XOR_LL_ITERATOR_INITIALISER;
    if (__XOR_LL_TRUE == is_head_pop) {
        xor_ll_iterate_htt (ll_ptr, &iter);
    }
    else {
        xor_ll_iterate_tth (ll_ptr, &iter);
    }

    //2. update data if required
    if (data) {
        data->ptr = iter.node_data.ptr;
        data->size = iter.node_data.size;
    }

    //3. remove the node
    return xor_ll_remove_node_iter (ll_ptr, &iter);
}
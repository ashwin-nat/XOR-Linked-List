# XOR-Linked-List
If you wanna use an unnecessarily complex/obtuse data structure for your project,
XOR-Linked-List might be what you want.

## Who even would use this (or a similar) library?

* Alphas who want to show the betas that their doubly linked lists use less memory
* Geniuses who want to demonstrate their superior understanding of bitwise arithmetic
* Masochists
* People working on embedded platforms who really have tight memory constraints and need the performance of doubly linked lists

## What is an XOR-Linked list?
A normal (single) linked list maintains one pointer in every node of the list.
This pointer will point to the next node in the list. This means that one cannot
traverse this list in the reverse order (since one can't know the previous path)
```
+-----------+       +-----------+       +----------+
|  Data     |       |  Data     |       |  Data    |
|           |       |           |       |          |
+-----------+       +-----------+       +----------+
|  Next ptr |       |  Next Ptr |       |  Next ptr|
|           +------->           +------>+          |
+-----------+       +-----------+       +----------+
```

To overcome this problem, we can use a doubly linked list. In this, all nodes 
will maintain two pointers.

```
+---------------+        +----------------+       +----------------+
|               |        |                |       |                |
|   Data        |        |    Data        |       |     Data       |
|               |        |                |       |                |
+-------+-------+        +--------+-------+       +-------+--------+
| prev  | next  +--------> prev   | next  +------>+ prev  | next   |
|       |       |        |        |       |       |       |        |
|       |       <--------+        |       <-------+       |        |
+-------+-------+        +--------+-------+       +-------+--------+

```

But the issue with this approach is that there is one pointer extra to be stored
per node (in most modern systems this shouldn't really be an issue)

An XOR-Linked list is a data structure that gets the best of the above worlds.
It is a linked list that can be traversed in either direction, but only needs 
one "pointer". 

This is achieved by using the magical property of XOR.
```
if:
    A XOR B = C
then
    A XOR C = B
    B XOR C = A
```

So at every node, we will store the XOR of the previous and current node.
```
    xor_ptr = &(PREV) XOR &(NEXT)
```
So while traversing, all we need to do is perform
```
    next = &(PREV) XOR (CURR->xor_ptr)
```

## Running the example:
Use GNU make to build the example application.
```
make
./xor-ll
```

## Linked list usage
Just copy xor-ll.c in your sources directory and xor-ll.h in your includes
directory.

First, create an XOR Linked list object on the stack or the heap. If created on
the stack, it can be initialised using the below macro
```
XOR_LL my_ll = XOR_LL_INITIALISER;
```
Otherwise, it can also be initialised using the init function as shown below
```
xor_ll_init (&my_ll);
```

### Insertion
For inserting items, use the push_head and push_tail functions.

**NOTE: The push functions do not allocate the given data and make a copy, 
they just stored the provided pointer and size**
```
xor_ll_push_tail (&my_ll, &b, sizeof(b));
xor_ll_push_head (&my_ll, &c, sizeof(c));
```

Support for inserting in between is pending

### Traversal
For traversing the list, there are two directions
* Head to Tail (HTT)
* Tail to Head (TTH)

Create an iterator and use a while loop like shown below
```
XOR_LL_ITERATOR iter = XOR_LL_ITERATOR_INITIALISER;
while (XOR_LL_STATUS_EOL != xor_ll_iterate_htt(&my_ll, &iter)) {
    //the node data will be present in iter.node_data
    //you should access iter.node_data.ptr and iter.node_data.size
}
```
Likewise, for the opposite direction, use xor_ll_iterate_tth()

There is also a convenience macro available for the same
```
XOR_LL_ITERATOR iter = XOR_LL_ITERATOR_INITIALISER;
XOR_LL_LOOP_HTT(&my_ll,&my_iter) {
    //every node can be processed here
}
```

### Deletion
For deleting a node, we can use the xor_ll_pop_head() and xor_ll_pop_tail()
functions.

But if you want to delete a node while traversal, you can use the 
xor_ll_remove_node_iter() function (refer to example.c)


### Destruction
Once done with the linked list, don't forget to destroy it before it goes out of
scope. This can be done using the xor_ll_destroy() function.
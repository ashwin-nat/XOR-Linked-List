# XOR-Linked-List
This library can be used to stored data of any type in an XOR linked list.
The user can also traverse the list in forward and reverse directions.

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

This is achieved by using the magic of XOR.
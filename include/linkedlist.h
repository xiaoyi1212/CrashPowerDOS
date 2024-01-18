#ifndef CPOS_LINKEDLIST_H
#define CPOS_LINKEDLIST_H

typedef struct LinkedNode{
    struct LinkedNode *head;
    void* ptr;
    struct LinkedNode *next;
}node_t;

typedef struct LinkedList{
    struct LinkedNode *head;
    struct LinkedNode *end;
} list_t;

#endif

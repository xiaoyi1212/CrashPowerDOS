#include "linkedlist.h"
#include "memory.h"

list_t *manager;

struct LinkedNode *create_node(void *ptr){
    if(manager == NULL){
        manager = (list_t*) kmalloc(sizeof(list_t));
        manager->head = NULL;
        manager->end = NULL;
    }

    struct LinkedNode *node = (struct LinkedNode*) kmalloc(sizeof(struct LinkedNode));
    if(manager->head == NULL){
        manager->head = node;
    }

    node->head = manager->end;
    node->ptr = ptr;
    node->next = NULL;



    return node;
}

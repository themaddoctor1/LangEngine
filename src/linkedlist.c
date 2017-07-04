#include "linkedlist.h"

#include <stdlib.h>
#include <stdio.h>

struct llnode {
    struct llnode *prev;
    struct llnode *next;
    void *item;
};

typedef struct llnode* LLnode;

struct linkedlist {
    LLnode head;
    LLnode tail;
    long size;
};

/**
 * Creates a node for use in a linked list.
 */
LLnode makeLLnode(LLnode prev, void *item, LLnode next) {
    LLnode node = (LLnode) malloc(sizeof(struct linkedlist));
    node->prev = prev;
    node->next = next;
    node->item = item;

    return node;
}

void* disposeLLnode(LLnode node) {
    void *res = node->item;
    node->item = NULL;
    node->prev = NULL;
    node->next = NULL;

    free(node);
    return res;
}

LinkedList makeLinkedList() {
    LinkedList list = (LinkedList) malloc(sizeof(struct linkedlist));

    // Initial conditions
    list->head = list->tail = NULL;
    list->size = 0;

    return list;
}

LinkedList buildLinkedList(void **items) {
    LinkedList list = makeLinkedList();
    int i;
    for (i = 0; items[i]; i++)
        enqueue(list, items[i]);

    return list;
}

void disposeLinkedList(LinkedList ll) {
    // Remove everything, then free the pointer (pop frees nodes).
    while (ll->head)
        pop(ll);

    free(ll);
}

void push(LinkedList ll, void *item) {
    // NULL check.
    if (!ll) return;

    ll->head = makeLLnode(NULL, item, ll->head);
    if (!ll->tail)
        // List is empty, node is new front.
        ll->tail = ll->head;

    ll->size++;
}


void* pop(LinkedList ll) {
    // Empty case.
    if (!ll || !ll->head)
        return NULL;

    LLnode head = ll->head;
    
    /* Set new head */
    ll->head = head->next;

    /* Unlink it if necessary */
    if (ll->head)
        ll->head->prev = NULL;

    ll->size--;

    /* Dispose old head and return */
    return disposeLLnode(head);
}

void insertToLL(LinkedList ll, void *item, int idx) {
    if (!ll) return;

    if (idx <= 0) {
        // Insertion at index 0
        push(ll, item);
        return;
    } else if (idx >= ll->size) {
        // Insertion at end of list.
        enqueue(ll, item);
        return;
    }
    
    // Get node at idx
    LLnode curr = ll->head;
    while (curr && --idx)
        curr = curr->next;
    
    // Build the node.
    LLnode node = makeLLnode(curr, item, curr->next);

    /* Relink */
    if (curr->next)
        curr->next->prev = node;
    curr->next = node;
    
    // Increment list size.
    ll->size++;
}

void *removeFromLL(LinkedList ll, int idx) {
    if (!ll) return NULL;
    
    if (idx <= 0)
        // Remove front of list.
        return pop(ll);

    LLnode curr = ll->head;
    
    if (!curr)
        // Empty case
        return NULL;
    
    // Seek node at idx.
    while (curr->next && idx--)
        curr = curr->next;
    
    /* Relink */
    curr->prev->next = curr->next;
    if (curr->next)
        curr->next->prev = curr->prev;

    /* Extract and return */
    ll->size--;
    return disposeLLnode(curr);

}

void enqueue(LinkedList ll, void *item) {
    if (!ll) return;
    
    // Build node
    LLnode node = makeLLnode(ll->tail, item, NULL);
    
    // Apply new tail
    ll->tail = node;

    if (ll->head)
        // List is non-empty, apply linking.
        node->prev->next = node;
    else
        // List is empty
        ll->head = ll->tail;

    ll->size++;
}

void* dequeue(LinkedList ll) {
    // In this structure, dequeue is identical to pop.
    return pop(ll);
}


long sizeOfLinkedList(LinkedList ll) {
    return ll->size;
}


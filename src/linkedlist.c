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
    while (ll->head)
        pop(ll);

    free(ll);
}

void push(LinkedList ll, void *item) {
    ll->head = makeLLnode(NULL, item, ll->head);
    if (!ll->tail)
        ll->tail = ll->head;

    ll->size++;
}

void* pop(LinkedList ll) {
    if (!ll->head)
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
    if (!idx) {
        push(ll, item);
        return;
    } else if (idx >= ll->size) {
        enqueue(ll, item);
        return;
    }

    LLnode curr = ll->head;
    while (curr && --idx)
        curr = curr->next;

    LLnode node = makeLLnode(curr, item, curr->next);

    /* Relink */
    if (curr->next)
        curr->next->prev = node;
    curr->next = node;

    ll->size++;
}

void *removeFromLL(LinkedList ll, int idx) {
    if (!idx)
        return pop(ll);

    LLnode curr = ll->head;
    
    if (!curr)
        return NULL;

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
    LLnode node = makeLLnode(ll->tail, item, NULL);
    
    ll->tail = node;
    if (ll->head)
        node->prev->next = node;
    else
        ll->head = ll->tail;

    ll->size++;
}

void* dequeue(LinkedList ll) {
    return pop(ll);
}


long sizeOfLinkedList(LinkedList ll) {
    return ll->size;
}


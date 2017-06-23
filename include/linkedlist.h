#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

struct linkedlist;
typedef struct linkedlist* LinkedList;

LinkedList makeLinkedList();
LinkedList buildLinkedList(void**);
void disposeLinkedList(LinkedList ll);

void push(LinkedList ll, void *item);
void* pop(LinkedList ll);

void insertToLL(LinkedList ll, void *item, int idx);
void* removeFromLL(LinkedList ll, int idx);

void enqueue(LinkedList ll, void *item);
void* dequeue(LinkedList ll);

long sizeOfLinkedList(LinkedList ll);

#endif

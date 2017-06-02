#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

struct linkedlist;
typedef struct linkedlist* LinkedList;

LinkedList makeLinkedList();
void disposeLinkedList(LinkedList ll);

void push(LinkedList ll, void *item);
void* pop(LinkedList ll);

void insert(LinkedList ll, void *item, int idx);
void* remove(LinkedList ll, int idx);

void enqueue(LinkedList ll, void *item);
void* dequeue(LinkedList ll);

long sizeOfLinkedList(LinkedList ll);

#endif

#include <stddef.h>
// We have to use this one for malloc because we are weak
#include <stdlib.h>
#include "game_object.h"

void init(LinkedList list) {
    list.head = NULL;
    list.tail = NULL;
}

void insert(LinkedList list, void *pt) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->value = pt;
    node->next = NULL;
    if (list.head == NULL) {
        list.head = node;
    } else {
        list.tail->next = node;
        list.tail = node;
    }
}

*Node getElementAt(LinkedList list, int index) {
    int i = 0;
    Node *curent = list.head;

    if (list.head == NULL) return;
    while (i != index && current != NULL) {
        current = current.next;
        i++;
    }
    //Either null or something
    return current;
}

void removeAt(LinkedList list, int index) {
    int i = 0;
    Node *current = list.head;
    Node *prevNode = NULL;

    if (list.head == NULL) return;
    if (index == 0) {
        prevNode = list.head;
        list.head = list.head->next;
        free(prevNode);
        return;
    }
    while (i != index && current != list.tail) {
        prevNode = current;
        current = current->next;
        i++;
    }
    if (i == index) {
        prevNode->next = current->next;
        free(current);
    }
}

void freeList(LinkedList list) {
    Node *node = list.head;
    Node *prevNode = NULL;
    while (node != NULL) {
        free(node->value);
        prevNode = node;
        node = node->next;
        free(node);
    }
    free(prevNode);
}
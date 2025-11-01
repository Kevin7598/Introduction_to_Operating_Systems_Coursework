#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef struct Node {
    char *key;
    void *data;
    struct Node *next;
} Node;

typedef int (*CompareFunc)(const void *, const void *);

Node* create_node(const char *key, void *data, size_t data_size);
void append(Node **head, const char *key, void *data, size_t data_size);
void free_list(Node *head);
Node* search(Node *head, const char *key);
void merge_sort(Node **headRef, CompareFunc cmp);

int cmp_int_inc(const void *a, const void *b);
int cmp_int_dec(const void *a, const void *b);
int cmp_double_inc(const void *a, const void *b);
int cmp_double_dec(const void *a, const void *b);
int cmp_str_inc(const void *a, const void *b);
int cmp_str_dec(const void *a, const void *b);
int cmp_random(const void *a, const void *b);

#endif

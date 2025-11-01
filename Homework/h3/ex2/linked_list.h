# ifndef LINKED_LIST_H
# define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Node {
    char *key;
    void *data;
    struct Node *next;
} Node;

char* my_strdup(const char* s);

Node* create_node(const char *key, void *data, size_t data_size);

void free_list(Node *head);

Node* search(Node *head, const char *key);

typedef int (*CompareFunc)(const void*, const void*);

int cmp_int_inc(const void *a, const void *b);
int cmp_int_dec(const void *a, const void *b);
int cmp_double_inc(const void *a, const void *b);
int cmp_double_dec(const void *a, const void *b);
int cmp_str_inc(const void *a, const void *b);
int cmp_str_dec(const void *a, const void *b);
int cmp_random(const void *a, const void *b);

int get_file_type(const char *filename);

CompareFunc get_compare_func(const char *sort_type, int datatype);

void append(Node **head, const char *key, void *data, size_t data_size);

Node* read_file(const char *filename, int datatype);
void write_file(const char *filename, Node *head, int datatype);

Node* merge(Node *a, Node *b, CompareFunc cmp);
void split(Node *source, Node **frontRef, Node **backRef);
void merge_sort(Node **head_ref, CompareFunc cmp);

# endif

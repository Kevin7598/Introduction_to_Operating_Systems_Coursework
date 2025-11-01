#include "linked_list.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char* my_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* copy = malloc(len + 1);
    if (copy) memcpy(copy, s, len + 1);
    return copy;
}

Node* create_node(const char *key, void *data, size_t data_size) {
    Node *new_node = malloc(sizeof(Node));
    new_node->key = my_strdup(key);
    new_node->data = malloc(data_size);
    memcpy(new_node->data, data, data_size);
    new_node->next = NULL;
    return new_node;
}

void append(Node **head, const char *key, void *data, size_t data_size) {
    Node *new_node = create_node(key, data, data_size);
    if (!*head) {
        *head = new_node;
        return;
    }
    Node *temp = *head;
    while (temp->next) temp = temp->next;
    temp->next = new_node;
}

void free_list(Node *head) {
    while (head) {
        Node *temp = head;
        head = head->next;
        free(temp->key);
        free(temp->data);
        free(temp);
    }
}

Node* search(Node *head, const char *key) {
    while (head) {
        if (strcmp(head->key, key) == 0) return head;
        head = head->next;
    }
    return NULL;
}

static Node* merge(Node* a, Node* b, CompareFunc cmp) {
    if (!a) return b;
    if (!b) return a;
    Node* result = NULL;
    if (cmp(a->data, b->data) <= 0) {
        result = a;
        result->next = merge(a->next, b, cmp);
    } else {
        result = b;
        result->next = merge(a, b->next, cmp);
    }
    return result;
}

static void split(Node* source, Node** frontRef, Node** backRef) {
    Node *slow = source;
    Node *fast = source->next;
    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

void merge_sort(Node** headRef, CompareFunc cmp) {
    Node* head = *headRef;
    if (!head || !head->next) return;
    Node *a, *b;
    split(head, &a, &b);
    merge_sort(&a, cmp);
    merge_sort(&b, cmp);
    *headRef = merge(a, b, cmp);
}

int cmp_int_inc(const void *a, const void *b) { return (*(int*)a - *(int*)b); }
int cmp_int_dec(const void *a, const void *b) { return (*(int*)b - *(int*)a); }
int cmp_double_inc(const void *a, const void *b) { double diff = (*(double*)a - *(double*)b); return (diff>0)-(diff<0); }
int cmp_double_dec(const void *a, const void *b) { double diff = (*(double*)b - *(double*)a); return (diff>0)-(diff<0); }
int cmp_str_inc(const void *a, const void *b) { return strcmp(a,b); }
int cmp_str_dec(const void *a, const void *b) { return strcmp(b,a); }
int cmp_random(const void *a, const void *b) { (void)a; (void)b; return rand()%3 - 1; }

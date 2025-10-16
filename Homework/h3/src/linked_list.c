# include "linked_list.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <time.h>

char* my_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

Node* create_node(const char *key, void *data, size_t data_size) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node->key = strdup(key);
    new_node->data = malloc(data_size);
    memcpy(new_node->data, data, data_size);
    new_node->next = NULL;
    return new_node;
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

int cmp_int_inc(const void *a, const void *b) { return (*(int*)a - *(int*)b); }
int cmp_int_dec(const void *a, const void *b) { return (*(int*)b - *(int*)a); }
int cmp_double_inc(const void *a, const void *b) {
    double diff = (*(double*)a - *(double*)b);
    return (diff > 0) - (diff < 0);
}
int cmp_double_dec(const void *a, const void *b) {
    double diff = (*(double*)b - *(double*)a);
    return (diff > 0) - (diff < 0);
}
int cmp_str_inc(const void *a, const void *b) { return strcmp((const char*)a, (const char*)b); }
int cmp_str_dec(const void *a, const void *b) { return strcmp((const char*)b, (const char*)a); }
int cmp_random(const void *a, const void *b) { return rand() % 3 - 1; }

int get_file_type(const char *filename) {
    if (strstr(filename, "int")) return 1;
    if (strstr(filename, "double")) return 2;
    if (strstr(filename, "char*")) return 3;
    return 0;
}

CompareFunc get_compare_func(const char *sort_type, int datatype) {
    if (strcmp(sort_type, "rand") == 0) return cmp_random;
    if (datatype == 1) return strcmp(sort_type, "inc") == 0 ? cmp_int_inc : cmp_int_dec;
    if (datatype == 2) return strcmp(sort_type, "inc") == 0 ? cmp_double_inc : cmp_double_dec;
    if (datatype == 3) return strcmp(sort_type, "inc") == 0 ? cmp_str_inc : cmp_str_dec;
    return NULL;
}

void append(Node **head, const char *key, void *data, size_t data_size) {
    Node *new_node = create_node(key, data, data_size);

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    Node *temp = *head;
    while (temp->next != NULL) temp = temp->next;

    temp->next = new_node;
}

Node* read_file(const char *filename, int datatype) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { perror("fopen"); exit(1); }

    Node *head = NULL;
    char line[256], key[128], val[128];
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^=]=%s", key, val) != 2) continue;

        if (datatype == 1) {
            int x = atoi(val);
            append(&head, key, &x, sizeof(int));
        } else if (datatype == 2) {
            double d = atof(val);
            append(&head, key, &d, sizeof(double));
        } else if (datatype == 3) {
            char *s = val;
            // printf("Read string: %s=%s\n", key, s); // Debug print
            append(&head, key, s, strlen(s) + 1);
        }
    }
    fclose(fp);
    return head;
}


void write_file(const char *filename, Node *head, int datatype) {
    FILE *fp = fopen(filename, "w");
    if (!fp) { 
        perror("fopen");
        exit(1);
    }

    while (head) {
        if (datatype == 1) fprintf(fp, "%s=%d\n", head->key, *(int*)head->data);
        else if (datatype == 2) fprintf(fp, "%s=%g\n", head->key, *(double*)head->data);
        else if (datatype == 3) fprintf(fp, "%s=%s\n", head->key, (char*)head->data);
        head = head->next;
    }
    fclose(fp);
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

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
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

    Node* a;
    Node* b;

    split(head, &a, &b);
    // printf("Splitting list...\n"); // Debug print
    merge_sort(&a, cmp);
    merge_sort(&b, cmp);

    *headRef = merge(a, b, cmp);
}

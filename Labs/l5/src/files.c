#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            append(&head, key, val, strlen(val) + 1);
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

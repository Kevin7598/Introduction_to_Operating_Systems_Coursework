#include "lab5_dlist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

typedef struct dlistNode_t {
    char* key;
    dlistValue value;
    struct dlistNode_t* prev;
    struct dlistNode_t* next;
} dlistNode;

typedef struct dlistInternal_t {
    dlistValueType type;
    dlistNode* head;
    dlistNode* tail;
} dlistInternal;

static dlistValueType g_sortType;
static dlistSortMethod g_sortMethod;

static char* safe_strdup(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* p = malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static int cmpWrapper(const void *a, const void *b) {
    const dlistNode *na = *(const dlistNode **)a;
    const dlistNode *nb = *(const dlistNode **)b;

    if (g_sortMethod == DLIST_SORT_RAND) return (rand() % 2) ? -1 : 1;

    if (g_sortType == DLIST_INT) {
        long long diff = (long long)na->value.intValue - (long long)nb->value.intValue;
        return (g_sortMethod == DLIST_SORT_INC) ? (diff < 0 ? -1 : (diff > 0 ? 1 : 0))
                                               : (diff < 0 ? 1  : (diff > 0 ? -1 : 0));
    } 
    else if (g_sortType == DLIST_DOUBLE) {
        double da = na->value.doubleValue;
        double db = nb->value.doubleValue;
        if (da == db) return 0;
        int cmp = (da < db) ? -1 : 1;
        return (g_sortMethod == DLIST_SORT_INC) ? cmp : -cmp;
    } 
    else if (g_sortType == DLIST_STR) {
        int cmp = strcmp(na->value.strValue, nb->value.strValue);
        return (g_sortMethod == DLIST_SORT_INC) ? cmp : -cmp;
    }
    return 0;
}

dlist createDlist(dlistValueType type) {
    dlistInternal* list = malloc(sizeof(dlistInternal));
    if (!list) return NULL;
    list->type = type;
    list->head = list->tail = NULL;
    return list;
}

int dlistIsEmpty(dlist_const this) {
    const dlistInternal* list = (const dlistInternal*) this;
    if (!list) return 1;
    return (list->head == NULL);
}

void dlistAppend(dlist this, const char* key, dlistValue value) {
    dlistInternal* list = (dlistInternal*) this;
    if (!list || !key) return;

    dlistNode* node = malloc(sizeof(dlistNode));
    if (!node) return;

    node->key = safe_strdup(key);
    node->prev = list->tail;
    node->next = NULL;

    if (list->type == DLIST_INT) node->value.intValue = value.intValue;
    else if (list->type == DLIST_DOUBLE) node->value.doubleValue = value.doubleValue;
    else if (list->type == DLIST_STR) node->value.strValue = safe_strdup(value.strValue);

    if (list->tail) list->tail->next = node;
    else list->head = node;
    list->tail = node;
}

void dlistPrint(dlist_const this) {
    const dlistInternal* list = (const dlistInternal*) this;
    if (!list) return;

    for (dlistNode* n = list->head; n; n = n->next) {
        if (list->type == DLIST_INT) printf("%s=%d\n", n->key, n->value.intValue);
        else if (list->type == DLIST_DOUBLE) printf("%s=%g\n", n->key, n->value.doubleValue);
        else if (list->type == DLIST_STR) printf("%s=%s\n", n->key, n->value.strValue);
    }
}

static void free_nodes(dlistInternal* list) {
    if (!list) return;
    dlistNode* curr = list->head;
    while (curr) {
        dlistNode* tmp = curr;
        curr = curr->next;
        free(tmp->key);
        if (list->type == DLIST_STR) free(tmp->value.strValue);
        free(tmp);
    }
    list->head = list->tail = NULL;
}

void dlistFree(dlist this) {
    dlistInternal* list = (dlistInternal*) this;
    if (!list) return;
    free_nodes(list);
    free(list);
}

static size_t count_nodes(const dlistInternal* list) {
    if (!list) return 0;
    size_t c = 0;
    for (dlistNode* n = list->head; n; n = n->next) c++;
    return c;
}

void dlistSort(dlist_const src, dlist dst, dlistSortMethod method) {
    const dlistInternal* listSrc = (const dlistInternal*) src;
    dlistInternal* listDst = (dlistInternal*) dst;

    if (!listSrc || !listDst) return;
    if (listSrc->type != listDst->type) return;

    size_t count = count_nodes(listSrc);
    if (count == 0) return;

    dlistNode** arr = malloc((size_t)count * sizeof(dlistNode*));
    if (!arr) return;

    size_t i = 0;
    for (dlistNode* n = listSrc->head; n; n = n->next) arr[i++] = n;

    g_sortType = listSrc->type;
    g_sortMethod = method;
    srand((unsigned)time(NULL) ^ (unsigned)(uintptr_t)arr);

    qsort(arr, count, sizeof(dlistNode*), cmpWrapper);

    free_nodes(listDst);

    for (i = 0; i < count; ++i) {
        dlistValue v;
        if (listSrc->type == DLIST_INT) v.intValue = arr[i]->value.intValue;
        else if (listSrc->type == DLIST_DOUBLE) v.doubleValue = arr[i]->value.doubleValue;
        else v.strValue = arr[i]->value.strValue;
        dlistAppend(listDst, arr[i]->key, v);
    }

    free(arr);
}

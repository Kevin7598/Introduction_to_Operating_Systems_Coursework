#ifndef FILES_H
#define FILES_H

#include "linked_list.h"

int get_file_type(const char *filename);
CompareFunc get_compare_func(const char *sort_type, int datatype);
Node* read_file(const char *filename, int datatype);
void write_file(const char *filename, Node *head, int datatype);

#endif

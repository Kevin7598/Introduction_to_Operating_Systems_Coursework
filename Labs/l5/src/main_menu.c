#include "ui.h"
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int run_menu(void) {
    srand(time(NULL));
    printf("Welcome to the Sorting Program!\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1. Sort a file\n");
        printf("2. Exit\n");
        printf("Choose an option: ");

        int choice;
        scanf("%d", &choice);
        getchar(); // consume newline

        if (choice == 2) break;
        if (choice != 1) { printf("Invalid choice.\n"); continue; }

        char filename[128], sorttype[16];
        printf("Enter input file name: ");
        scanf("%s", filename);
        printf("Enter sort type (inc, dec, rand): ");
        scanf("%s", sorttype);

        int datatype = get_file_type(filename);
        if (!datatype) { printf("Unknown file type.\n"); continue; }

        Node *list = read_file(filename, datatype);
        CompareFunc cmp = get_compare_func(sorttype, datatype);
        merge_sort(&list, cmp);

        char outputfile[128];
        snprintf(outputfile, sizeof(outputfile), "%s_%s.txt", sorttype,
                 datatype == 1 ? "int" : (datatype == 2 ? "double" : "char*"));
        write_file(outputfile, list, datatype);
        printf("File sorted and written to %s\n", outputfile);
        free_list(list);
    }
    printf("Goodbye!\n");
    return 0;
}
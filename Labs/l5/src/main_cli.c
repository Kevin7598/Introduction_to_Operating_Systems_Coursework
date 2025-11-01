#include "ui.h"
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int run_cli(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <inputfile> <sorttype>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));
    const char *inputfile = argv[1];
    const char *sorttype = argv[2];

    int datatype = get_file_type(inputfile);
    if (datatype == 0) {
        fprintf(stderr, "Unknown data type in filename.\n");
        return 1;
    }

    printf("Reading %s\n", inputfile);
    Node *list = read_file(inputfile, datatype);

    printf("Sorting elements...\n");
    CompareFunc cmp = get_compare_func(sorttype, datatype);
    merge_sort(&list, cmp);

    char outputfile[128];
    snprintf(outputfile, sizeof(outputfile), "%s_%s.txt", sorttype,
             datatype == 1 ? "int" : (datatype == 2 ? "double" : "char*"));

    printf("Writing %s\n", outputfile);
    write_file(outputfile, list, datatype);

    free_list(list);
    return 0;
}

# include <stdio.h>
# include <time.h>
# include <string.h>
# include <stdlib.h>
# include "linked_list.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <inputfile> <sorttype>\n", argv[0]);
        return 1;
    }

    srand((unsigned int) time(NULL));

    const char *inputfile = argv[1];
    const char *sorttype = argv[2];

    int datatype = get_file_type(inputfile);
    if (datatype == 0) {
        fprintf(stderr, "Unknown data type in filename.\n");
        return 1;
    }

    printf("reading %s\n", inputfile);
    Node *list = read_file(inputfile, datatype);

    printf("sorting elements\n");
    CompareFunc cmp = get_compare_func(sorttype, datatype);
    merge_sort(&list, cmp);

    char outputfile[128];
    snprintf(outputfile, sizeof(outputfile), "%s_%s.txt", sorttype,
            datatype == 1 ? "int" : (datatype == 2 ? "double" : "char*"));

    printf("writing %s\n", outputfile);
    write_file(outputfile, list, datatype);

    free_list(list);
    return 0;
}

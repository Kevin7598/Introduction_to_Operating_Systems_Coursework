#include "ui.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--menu") == 0) {
        return run_menu();
    } else {
        return run_cli(argc, argv);
    }
}
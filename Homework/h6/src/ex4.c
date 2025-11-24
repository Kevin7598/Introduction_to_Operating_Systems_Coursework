#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    size_t size_mb = 1024;
    size_t bytes = size_mb * 1024UL * 1024UL;
    size_t step = sysconf(_SC_PAGESIZE);
    char *a = malloc(bytes);
    char *b = malloc(bytes);
    if (!a || !b) { perror("malloc"); return 1; }
    printf("Buffers: %zu MB each; touching every %zu bytes (page size). Ctrl-C to stop.\n",
           size_mb, step);
    for (;;) {
        for (size_t i = 0; i < bytes; i += step) a[i] = 1;
        for (size_t i = 0; i < bytes; i += step) b[i] = 2;
    }
    return 0;
}

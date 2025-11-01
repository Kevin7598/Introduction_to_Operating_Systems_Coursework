# ECE4820J Lab 6

### Kaiqi Zhu 522370910091

## Design the plugin architecture

### Design a plugin architecture allowing to open various file-types

```cpp
typedef struct {
    const char *name;
    int  (*can_open)(const char *filename);
    int  (*open_file)(const char *filename, dlist list);
} Plugin;
```

### Refactor the code of the main application such that opening a text file becomes part of a plugin

```cpp
extern Plugin text_plugin;
extern Plugin csv_plugin;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    const char *filename = argv[1];

    Plugin *plugins[] = { &text_plugin, &csv_plugin, NULL };

    dlist list = createDlist(DLIST_INT);

    for (int i = 0; plugins[i]; i++) {
        if (plugins[i]->can_open(filename)) {
            printf("Using plugin: %s\n", plugins[i]->name);
            plugins[i]->open_file(filename, list);
            printf("File loaded successfully.\n");
            dlistPrint(list);
            dlistFree(list);
            return 0;
        }
    }

    printf("No suitable plugin found for %s\n", filename);
    dlistFree(list);
    return 1;
}
```

### Adjust the code such that text files can opened and processed

```cpp
int can_open(const char *filename) {
    return strstr(filename, ".txt") != NULL;
}

int open_file(const char *filename, dlist list) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';

        char *key = line;
        char *val = eq + 1;
        val[strcspn(val, "\r\n")] = 0;

        dlistValue v;
        v.intValue = atoi(val); 
        dlistAppend(list, key, v);
    }

    fclose(fp);
    return 1;
}

Plugin text_plugin = {
    .name = "text",
    .can_open = can_open,
    .open_file = open_file
};
```

### Write the skeleton of a plugin to open and process csv files

```cpp
int can_open(const char *filename) {
    return strstr(filename, ".csv") != NULL;
}

int open_file(const char *filename, dlist list) {
    printf("[csv plugin] Opening %s\n", filename);
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return 0;
    }

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        char *field = strtok(line, ",");
        while (field) {
            printf("[csv plugin] Field: %s\n", field);
            field = strtok(NULL, ",");
        }
    }

    fclose(fp);
    return 1;
}

Plugin csv_plugin = {
    .name = "csv",
    .can_open = can_open,
    .open_file = open_file
};
```

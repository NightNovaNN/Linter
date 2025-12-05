#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

// ------------------------------------------------------------
// Initialize dynamic error list
// ------------------------------------------------------------
void init_error_list(ErrorList *list) {
    list->count = 0;
    list->capacity = 8;
    list->items = (LintError *)malloc(sizeof(LintError) * list->capacity);
    if (!list->items) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

// ------------------------------------------------------------
// Add an error (dynamic resize if needed)
// ------------------------------------------------------------
void add_error(ErrorList *list, int line, int col, const char *msg) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->items = (LintError *)realloc(
            list->items, 
            sizeof(LintError) * list->capacity
        );
        if (!list->items) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
    }

    LintError *e = &list->items[list->count++];
    e->line = line;
    e->column = col;
    strncpy(e->message, msg, 255);
    e->message[255] = '\0';
}

// ------------------------------------------------------------
// Free error list memory
// ------------------------------------------------------------
void free_error_list(ErrorList *list) {
    if (list->items) {
        free(list->items);
        list->items = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}

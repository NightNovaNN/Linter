#ifndef ERROR_H
#define ERROR_H

typedef struct {
    int line;
    int column;
    char message[256];
} LintError;

typedef struct {
    LintError *items;
    int count;
    int capacity;
} ErrorList;

void init_error_list(ErrorList *list);
void add_error(ErrorList *list, int line, int col, const char *msg);
void free_error_list(ErrorList *list);

// 🔥 Language enum
typedef enum {
    LANG_C = 1,
    LANG_CPP = 2,
    LANG_JAVA = 3,
    LANG_PYTHON = 4
} Language;

#endif

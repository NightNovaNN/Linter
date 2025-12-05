#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "error.h"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

//
// ------------------------------------------------------------
// POSITION TRACKER
// ------------------------------------------------------------
static void track_position(char c, int *line, int *col) {
    if (c == '\n') {
        (*line)++;
        *col = 1;
    } else {
        (*col)++;
    }
}

//
// ------------------------------------------------------------
// QUOTE SCANNER (single + double)
// ------------------------------------------------------------
void scan_quotes(const char *src, ErrorList *list) {
    int line = 1, col = 1;
    int in_single = 0, in_double = 0;
    int single_line = 0, single_col = 0;
    int double_line = 0, double_col = 0;

    for (int i = 0; src[i]; i++) {
        char c = src[i];

        if (c == '\'' && !in_double) {
            if (!in_single) {
                in_single = 1;
                single_line = line;
                single_col = col;
            } else {
                in_single = 0;
            }
        }

        if (c == '"' && !in_single) {
            if (!in_double) {
                in_double = 1;
                double_line = line;
                double_col = col;
            } else {
                in_double = 0;
            }
        }

        track_position(c, &line, &col);
    }

    if (in_single)
        add_error(list, single_line, single_col, "Unclosed single quote");

    if (in_double)
        add_error(list, double_line, double_col, "Unclosed double quote");
}

//
// ------------------------------------------------------------
// BRACKET SCANNER
// ------------------------------------------------------------
void scan_brackets(const char *src, ErrorList *list) {
    int line = 1, col = 1;
    int round = 0, curly = 0, square = 0;
    int round_line = 0, round_col = 0;
    int curly_line = 0, curly_col = 0;
    int square_line = 0, square_col = 0;

    for (int i = 0; src[i]; i++) {
        char c = src[i];

        if (c == '(') { round++; round_line = line; round_col = col; }
        if (c == ')') round--;

        if (c == '{') { curly++; curly_line = line; curly_col = col; }
        if (c == '}') curly--;

        if (c == '[') { square++; square_line = line; square_col = col; }
        if (c == ']') square--;

        track_position(c, &line, &col);
    }

    if (round > 0)
        add_error(list, round_line, round_col, "Unclosed '(' parenthesis");

    if (curly > 0)
        add_error(list, curly_line, curly_col, "Unclosed '{' brace");

    if (square > 0)
        add_error(list, square_line, square_col, "Unclosed '[' bracket");
}

//
// ------------------------------------------------------------
// C / C++ / Java COMMENT SCANNER (/* */)
// ------------------------------------------------------------
void scan_c_style_comments(const char *src, ErrorList *list) {
    int line = 1, col = 1;
    int in_comment = 0;
    int open_line = 0, open_col = 0;

    for (int i = 0; src[i]; i++) {
        char c = src[i];

        if (!in_comment && c == '/' && src[i+1] == '*') {
            in_comment = 1;
            open_line = line;
            open_col = col;
        }

        else if (in_comment && c == '*' && src[i+1] == '/') {
            in_comment = 0;
            i++; 
            col++;
        }

        track_position(c, &line, &col);
    }

    if (in_comment)
        add_error(list, open_line, open_col, "Unclosed /* */ comment");
}

//
// ------------------------------------------------------------
// PYTHON COMMENT: #
// ------------------------------------------------------------
void scan_python_comments(const char *src, ErrorList *list) {
    (void)list; 
    // Python '#' comment does not need closure, so nothing to check.
}

//
// ------------------------------------------------------------
// PYTHON TRIPLE QUOTES
// ------------------------------------------------------------
void scan_python_triple_quotes(const char *src, ErrorList *list) {
    int line = 1, col = 1;
    int in_triple = 0;
    int open_line = 0, open_col = 0;
    char triple_type = 0;

    for (int i = 0; src[i]; i++) {

        if (!in_triple && src[i] == '"' && src[i+1] == '"' && src[i+2] == '"') {
            in_triple = 1;
            triple_type = '"';
            open_line = line;
            open_col = col;
        }

        else if (!in_triple && src[i] == '\'' && src[i+1] == '\'' && src[i+2] == '\'') {
            in_triple = 1;
            triple_type = '\'';
            open_line = line;
            open_col = col;
        }

        else if (in_triple && triple_type == '"' &&
                 src[i] == '"' && src[i+1] == '"' && src[i+2] == '"') {
            in_triple = 0;
        }

        else if (in_triple && triple_type == '\'' &&
                 src[i] == '\'' && src[i+1] == '\'' && src[i+2] == '\'') {
            in_triple = 0;
        }

        track_position(src[i], &line, &col);
    }

    if (in_triple)
        add_error(list, open_line, open_col, "Unclosed Python triple quote");
}

//
// ------------------------------------------------------------
// PYTHON BASIC INDENT CHECKER
// ------------------------------------------------------------
void scan_python_indent(const char *src, ErrorList *list) {
    int line = 1;
    int indent_level = 0;

    int i = 0;
    while (src[i]) {

        int col = 1;
        int spaces = 0;

        while (src[i] == ' ') {
            spaces++;
            i++;
            col++;
        }

        if (src[i] != '\n' && src[i] != '\0') {
            if (spaces % 4 != 0) {
                add_error(list, line, col, "Indentation not multiple of 4");
            }
        }

        while (src[i] && src[i] != '\n') {
            i++;
        }

        if (src[i] == '\n') {
            i++;
            line++;
        }
    }
}

//
// ------------------------------------------------------------
// MULTI-LANGUAGE ENTRY POINT
// ------------------------------------------------------------
EXPORT int scan_all_errors(const char *src, ErrorList *out_list, int lang) {
    init_error_list(out_list);

    scan_quotes(src, out_list);
    scan_brackets(src, out_list);

    switch (lang) {
        case LANG_C:
        case LANG_CPP:
        case LANG_JAVA:
            scan_c_style_comments(src, out_list);
            break;

        case LANG_PYTHON:
            scan_python_comments(src, out_list);
            scan_python_triple_quotes(src, out_list);
            scan_python_indent(src, out_list);
            break;
    }

    return out_list->count;
}

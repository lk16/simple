#include <malloc.h>
#include <string.h>

#include "simple_types.h"

struct simple_error {
    struct simple_error *child;
    const char *filename;
    size_t line;
    const char *function;
    char *message;
};

struct simple_error *simple_error_new(const char *filename, size_t line, const char *function, const char *format, ...){
    struct simple_error *error = malloc(sizeof *error);

    *error = (struct simple_error){
        .filename = filename,
        .line = line,
        .function = function,
        .child = NULL,
        .message = NULL
    };

    char *buff = NULL;
    va_list args, args2;
    va_start(args, format);
    va_copy(args2, args);
    size_t message_length = (size_t)vsnprintf(buff, 0, format, args);
    va_end (args);

    error->message = malloc(message_length + 1);

    va_start(args2, format);
    vsnprintf(error->message, message_length, format, args2);
    va_end (args2);

    return error;
}

void simple_error_set_child(struct simple_error *error, struct simple_error *child) {
    error->child = child;
}

static void simple_error_show_line(const struct simple_error *error, FILE *file) {
    fprintf(file, "%s:%zu %s() %s\n", error->filename, error->line, error->function, error->message);
}

void simple_error_show(const struct simple_error *error, FILE *file) {
    fprintf(file, "%s\n", "An error occurred, stacktrace:");
    while(error) {
        simple_error_show_line(error, file);
        error = error->child;
    }
}

void simple_error_destroy(struct simple_error *error) {
    if(error->child) {
        simple_error_destroy(error->child);
    }
    free(error->message);
    free(error);
}

struct simple_string {
    char *cstring;
    size_t length;
};

struct simple_string *simple_string_new(const char *cstring) {
    struct simple_string *string = malloc(sizeof *string);
    string->length = strlen(cstring);
    string->cstring = malloc(string->length + 1);
    memcpy(string->cstring, cstring, string->length + 1);
    return string;
}

void  simple_string_destroy(struct simple_string *string) {
    free(string->cstring);
    free(string);
}

const char *simple_string_get(const struct simple_string *string) {
    return string->cstring;
}

bool simple_string_equals(const struct simple_string *lhs, const struct simple_string *rhs) {
    if(lhs->length != rhs->length) {
        return false;
    }
    return strcmp(lhs->cstring, rhs->cstring) == 0;
}

bool simple_string_startswith(const struct simple_string *string, const char *search) {
    return strncmp(string->cstring, search, strlen(search)) == 0;
}

size_t simple_string_hash(const struct simple_string *string);


struct simple_hashtable;

struct simple_hashtable *simple_hashtable_new(const struct type *key_type, const struct type *value_type);
void simple_hashtable_destroy(struct simple_hashtable *table);
struct simple_error *simple_hashtable_insert(struct simple_hashtable *table, const struct object *key, const struct object *value);
struct simple_error *simple_hashtable_find(struct simple_hashtable *table, const struct object *key, struct object **result);
struct simple_error *simple_hashtable_find_const(const struct simple_hashtable *table, const struct object *key, const struct object **result);
struct simple_error * simple_hashtable_upsert(struct simple_hashtable *table, const struct object *key, const struct object *value, bool *inserted);
struct simple_error *simple_hashtable_erase(struct simple_hashtable *table, const struct object *key, bool *erased);
size_t simple_hashtable_size(const struct simple_hashtable *table);

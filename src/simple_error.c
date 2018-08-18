#include "simple_error.h"

#include <malloc.h>

struct simple_error {
    struct simple_error *cause;
    const char *filename;
    size_t line;
    const char *function;
    char *message;
};

struct simple_error *simple_error_new_full(
    struct simple_error *cause,
    const char *filename,
    size_t line,
    const char *function,
    const char *format,
    ...
) {
    struct simple_error *error = calloc(1, sizeof *error);

    *error = (struct simple_error) {
        .filename = filename,
        .line = line,
        .function = function,
        .cause = cause,
        .message = NULL
    };

    va_list args, args2;
    va_start(args, format);
    size_t message_length = (size_t)vsnprintf(error->message, 0, format, args);
    va_end (args);

    error->message = calloc(message_length + 1, sizeof *error->message);

    va_start(args2, format);
    vsnprintf(error->message, message_length, format, args2);
    va_end (args2);

    return error;
}

void simple_error_show(
    const struct simple_error *error,
    FILE *file
) {
    if (!error) {
        return;
    }
    fprintf(file, "%s\n", "An error occurred, stacktrace:");
    while (error) {
        fprintf(file, "%s:%zu %s() %s\n", error->filename, error->line,
            error->function, error->message);
        error = error->cause;
    }
}

void simple_error_destroy(
    struct simple_error *error
) {
    if (error->cause) {
        simple_error_destroy(error->cause);
    }
    free(error->message);
    free(error);
}

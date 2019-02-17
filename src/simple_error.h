#pragma once

#include <stdio.h>

#define simple_error_new(format, ...) \
    simple_error_new_full(NULL, __FILE__, __LINE__, __FUNCTION__, \
        format, __VA_ARGS__)

#define simple_error_check(error) \
    if (error) { \
        error = simple_error_new_full(error, __FILE__, __LINE__, \
        __FUNCTION__, "%s", ""); \
        goto cleanup; \
    } \
    (void)0

struct simple_error;

struct simple_error *simple_error_new_full(
    struct simple_error *cause,
    const char *filename,
    size_t line,
    const char *function,
    const char *format,
    ...
) __attribute__((warn_unused_result));

void simple_error_show(
    const struct simple_error *error,
    FILE *file
);

void simple_error_destroy(
    struct simple_error *error
);

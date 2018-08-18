#pragma once

#include <stddef.h>
#include <stdbool.h>

struct simple_string;

struct simple_string *simple_string_new(
    const char *cstring
) __attribute__((warn_unused_result));

struct simple_string *simple_string_copy(
    const struct simple_string *string
) __attribute__((warn_unused_result));

void simple_string_destroy(
    struct simple_string *string
);

size_t simple_string_hash(
    const struct simple_string *string
);

bool simple_string_startswith(
    const struct simple_string *string,
    const char *search
);

bool simple_string_equals(
    const struct simple_string *lhs,
    const struct simple_string *rhs
);

const char *simple_string_get(
    const struct simple_string *string
);

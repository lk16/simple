#pragma once

#include <string.h>
#include <stdbool.h>

struct base_string;

struct base_string *base_string_new(const char *raw);

void base_string_destroy(struct base_string *bs);

size_t base_string_hash(const struct base_string *bs);

bool base_string_equals(
    const struct base_string *lhs,
    const struct base_string *rhs
);

const char *base_string_raw(const struct base_string *bs);

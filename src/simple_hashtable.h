#pragma once

#include <stddef.h>
#include <stdio.h>

#include "simple_error.h"
#include "simple_string.h"
#include "type.h"

struct simple_hashtable;

struct simple_hashtable *simple_hashtable_new(
    const struct type *key_type,
    const struct type *value_type
) __attribute__((warn_unused_result));

void simple_hashtable_destroy(
    struct simple_hashtable *table
);

struct simple_error *simple_hashtable_find(
    struct simple_hashtable *table,
    const struct object *key,
    struct object **result
) __attribute__((warn_unused_result));

struct simple_error *simple_hashtable_insert(
    struct simple_hashtable *table,
    const struct object *key,
    const struct object *value
) __attribute__((warn_unused_result));

struct simple_error *simple_hashtable_erase(
    struct simple_hashtable *table,
    const struct object *key,
    bool *erased
) __attribute__((warn_unused_result));

size_t simple_hashtable_size(
    const struct simple_hashtable *table
);

#pragma once

#include <stddef.h>

#include "base_string.h"
#include "object.h"

struct base_map *base_map_new(void);

void base_map_destroy(struct base_map *bm);

void base_map_set(
    struct base_map *bm,
    const struct base_string *key,
    struct object *value
);

struct object *base_map_get(
    struct base_map *bm,
    const struct base_string *key
);

struct base_map_iterator *base_map_iterator_new(
    struct base_map *bm,
    const struct base_string **key,
    struct object **value
);

bool base_map_iterator_next(
    struct base_map_iterator *bmi
);

void base_map_iterator_destroy(
    struct base_map_iterator *bmi
);
#pragma once

#include <stddef.h>

#include "base_string.h"

struct object;
struct base_map;

struct base_map *base_map_new();

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

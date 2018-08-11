#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "type.h"

struct base_map;
struct base_map_iterator;

struct base_map *base_map_new(void);
void base_map_destroy(struct base_map *bm);
void base_map_insert(struct base_map *bm, const char *key, struct object *value);
struct object *base_map_get(struct base_map *bm, const char *key);
const struct object *base_map_get_const(const struct base_map *bm, const char *key);
void base_map_remove(struct base_map *bm, const char *key);
size_t base_map_size(const struct base_map *bm);
struct base_map_iterator *base_map_iterate(struct base_map *bm, const char *key,
                                           struct object *value);

bool base_map_iterator_next(struct base_map_iterator *bmi);
void base_map_iterator_destroy(struct base_map_iterator *bmi);

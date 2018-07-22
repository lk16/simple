#pragma once

#include "base_string.h"

struct object;
struct type_registry;

struct object *object_new(
    const struct base_string *type_name,
    const struct base_string *instance_name,
    struct type_registry *tr
);

struct object *object_copy(const struct object *o);

struct object *object_get_attribute(struct object *o, const struct base_string *key);
struct object *object_set_attribute(struct object *o, const struct base_string *key, struct object *value);

struct object *object_get_type(struct object *o, const struct object *args);
struct object *object_has_type(struct object *o, const struct object *args);

struct object *object_get_parent(struct object *o, const struct object *args);
struct object *object_cast(struct object *o, const struct object *args);
struct object *object_assign(struct object *o, const struct object *args);

struct object *object_hash(const struct object *o);
struct object *object_equals(struct object *o, const struct object *args);
struct object *object_not_equals(struct object *o, const struct object *args);

struct object *object_less_than(struct object *o, const struct object *args);
struct object *object_equals(struct object *o, const struct object *args);
struct object *object_greater_than(struct object *o, const struct object *args);
struct object *object_greater_or_equal(struct object *o, const struct object *args);
struct object *object_less_or_equal(struct object *o, const struct object *args);

struct object *object_add(struct object *o, const struct object *args);
struct object *object_add_assign(struct object *o, const struct object *args);

struct object *object_subtract(struct object *o, const struct object *args);
struct object *object_subtract_assign(struct object *o, const struct object *args);

struct object *object_multiply(struct object *o, const struct object *args);
struct object *object_multiply_assign(struct object *o, const struct object *args);

struct object *object_divide(struct object *o, const struct object *args);
struct object *object_divide_assign(struct object *o, const struct object *args);

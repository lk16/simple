#pragma once

#include "base_string.h"

struct object;

struct object *object_new(const struct base_string *type_name);
struct object *object_copy(const struct object *o);

struct object *object_get_attribute(object *o, const struct object *args);
struct object *object_set_attribute(object *o, const struct object *args);

struct object *object_get_id(struct object *o, const struct object *args);
struct object *object_get_name(struct object *o, const struct object *args);

struct object *object_get_type(struct object *o, const struct object *args);
struct object *object_has_type(struct object *o, const struct object *args);

struct object *object_get_parent(struct object *o, const struct object *args);
struct object *object_cast(struct object *o, const struct object *args);

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




struct type_registry;
struct type;
struct type_operators;

struct type_registry *type_registry_new(void);
void type_registry_destroy(struct type_registry *tr);

struct type *type_registry_get_type(struct type_registry *tr, const struct base_string *type_name);

struct object *type_registry_get_object_type(struct type_registry *tr);
struct object *type_registry_get_object_null(struct type_registry *tr);
struct object *type_registry_get_object_true(struct type_registry *tr);
struct object *type_registry_get_object_false(struct type_registry *tr);
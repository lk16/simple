#pragma once

struct object;

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
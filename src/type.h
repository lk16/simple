#pragma once

#include <stdbool.h>
#include <stddef.h>

struct type;
struct object;
struct simple_error;
struct simple_string;

typedef struct object *(*memberfunc_t)(struct object *o, const struct object *args);

void type_registry_new(void);
void type_registry_destroy(void);
const struct type *type_registry_get_type(const char *type_name);
struct simple_error *type_registry_create_type(const char *type_name);
struct simple_error *type_registry_construct(const char *type_name, struct object **result);

struct simple_error *type_set_attribute(struct type *type, const char *key, struct object *value);

struct object *object_new_int(int value);
struct object *object_new_double(float value);
struct object *object_new_function(memberfunc_t value);
struct object *object_new_string(const char *format, ...);
struct object *object_new_type(const char *type_name);

void object_destroy(struct object *o);

struct simple_error *object_get_int(const struct object *o, int *result);
struct simple_error *object_get_double(const struct object *o, double *result);
const struct type *object_get_type(struct object *o);

struct simple_error *object_set_int(struct object *o, int value);
struct simple_error *object_set_double(struct object *o, double value);

size_t object_get_hash(const struct object *o);

struct simple_error *object_set_attribute(struct object *o, const char *key, struct object *value);
struct simple_error *object_get_attribute(struct object *o, const char *key, struct object **result);
struct simple_error *object_get_attribute_const(const struct object *o, const char *key,const struct object **result);
struct simple_error *object_call(struct object *o, const char *key, const struct object *args, struct object **result);
bool object_has_type(const struct object *o, const char *type_name);

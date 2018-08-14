#pragma once

#include <stdbool.h>
#include <stddef.h>

struct type;
struct object;
struct simple_error;
struct simple_string;

typedef struct simple_error *(*memberfunc_t)(
    struct object *o,
    const struct object *args,
    struct object **result
);

void type_registry_new(void);

void type_registry_destroy(void);

const struct type *type_registry_get_type(
    const char *type_name
) __attribute__((warn_unused_result));

struct simple_error *type_registry_create_type(
    const char *type_name,
    struct type **result
) __attribute__((warn_unused_result));

struct simple_error *type_registry_construct(
    const char *type_name,
    struct object **result
) __attribute__((warn_unused_result));

struct simple_error *type_set_attribute(
    struct type *type,
    const char *key,
    struct object *value
) __attribute__((warn_unused_result));

struct object *object_new_int(
    int value
) __attribute__((warn_unused_result));

struct object *object_new_double(
    float value
) __attribute__((warn_unused_result));

struct object *object_new_function(
    memberfunc_t value
) __attribute__((warn_unused_result));

struct object *object_new_string(
    const char *format,
    ...
) __attribute__((warn_unused_result));

struct object *object_copy(
    const struct object *o
) __attribute__((warn_unused_result));

void object_destroy(struct object *o);

bool object_has_type(
    const struct object *o,
    const struct type *type
);

const struct type *object_type(
    struct object *o
) __attribute__((warn_unused_result));

size_t object_get_hash(
    const struct object *o
);

bool object_equals(
    const struct object *lhs,
    const struct object *rhs
);

struct simple_error *object_get_int(
    const struct object *o,
    int *result
) __attribute__((warn_unused_result));

struct simple_error *object_get_double(
    const struct object *o,
    double *result
) __attribute__((warn_unused_result));

struct simple_error *object_get_string(
    const struct object *o,
    const struct simple_string **result
) __attribute__((warn_unused_result));

struct simple_error *object_set_int(
    struct object *o,
    int value
) __attribute__((warn_unused_result));

struct simple_error *object_set_double(
    struct object *o,
    double value
) __attribute__((warn_unused_result));

struct simple_error *object_set_attribute(
    struct object *o,
    const char *key,
    struct object *value
) __attribute__((warn_unused_result));

struct simple_error *object_get_attribute(
    struct object *o,
    const char *key,
    struct object **result
) __attribute__((warn_unused_result));

struct simple_error *object_get_attribute_const(
    const struct object *o,
    const char *key,
    const struct object **result
) __attribute__((warn_unused_result));

struct simple_error *object_call(
    struct object *o,
    const char *key,
    const struct object *args,
    struct object **result
) __attribute__((warn_unused_result));

#pragma once

#include <stddef.h>
#include <stdbool.h>

struct simple_error;
struct simple_string;
struct object;
struct type;

typedef struct simple_error *(*memberfunc_t)(
    struct object *o,
    const struct object *args,
    struct object **result
);

enum object_kind {
    OBJECT_INTEGER,
    OBJECT_STRING,
    OBJECT_FUNCTION,
    OBJECT_TYPE,
};

const char *object_kind_get_name(
    enum object_kind kind
);

struct object *object_new(
    enum object_kind kind,
    bool constant,
    const struct type *type
)__attribute__((warn_unused_result));

struct simple_error *object_new_int(
    int value,
    struct object **result
) __attribute__((warn_unused_result));

struct simple_error *object_new_function(
    memberfunc_t value,
    struct object **result
) __attribute__((warn_unused_result));

struct simple_error *object_new_string(
    struct object **result,
    const char *format,
    ...
) __attribute__((warn_unused_result));

struct simple_error *object_copy(
    const struct object *o,
    struct object **copy
) __attribute__((warn_unused_result));

struct simple_error *object_new_type(
    const char *type_name,
    enum object_kind instance_kind,
    struct object **result
) __attribute__((warn_unused_result));

void object_refcount_increase(
    struct object *o
);

void object_refcount_decrease(
    struct object *o
);

struct simple_error *object_has_type(
    const struct object *o,
    const struct type *type,
    bool *result
);

struct simple_error *object_check_type(
    const struct object *o,
    const struct type *type
);

const struct type *object_type(
    struct object *o
) __attribute__((warn_unused_result));

size_t object_get_hash(
    const struct object *o
);

struct simple_error *object_equals(
    const struct object *lhs,
    const struct object *rhs,
    bool *result
);

struct simple_error *object_get_int(
    const struct object *o,
    int *result
) __attribute__((warn_unused_result));

struct simple_error *object_set_int(
    struct object *o,
    int value
) __attribute__((warn_unused_result));

struct simple_error *object_get_type(
    const struct object *o,
    struct type **result
) __attribute__((warn_unused_result));


struct simple_error *object_get_string(
    struct object *o,
    struct simple_string **result
) __attribute__((warn_unused_result));

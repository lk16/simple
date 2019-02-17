#pragma once

#include <stdbool.h>
#include <stddef.h>

struct type_registry;
struct type;
struct object;
struct simple_error;
struct simple_string;
enum object_kind;

extern struct type_registry *registry;

struct simple_error *type_registry_new(
    void
) __attribute__((warn_unused_result));

struct simple_error *type_registry_destroy(
    void
) __attribute__((warn_unused_result));

struct simple_error *type_registry_get_type(
    const char *type_name,
    struct type **result
) __attribute__((warn_unused_result));

struct simple_error *type_registry_get_string_type(
    struct type **result
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

struct simple_error *type_new(
    const char *type_name,
    enum object_kind instance_kind,
    struct type **result
);

struct simple_error *type_get_name(
    const struct type *type,
    const char **result
);
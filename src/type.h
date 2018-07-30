#pragma once

void type_registry_new(void);
void type_registry_destroy(void);

struct type;
struct object; 
struct string;

const struct type *type_registry_get_type(
    const char *type_name
);

struct type *type_registry_create_type(
    const char *type_name
);

struct object *type_registry_instantiate(
    const char *type_name
);


struct object *object_set(
    struct object *o,
    const char *key,
    struct object *value
);

struct object *object_get(
    struct object *o,
    const char *key
);

struct object *object_call(
    struct object *o,
    const char *key,
    const struct object *args
);

const struct type *object_type(
    struct object *o
);

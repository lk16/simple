#pragma once

#include "base_string.h"

void type_registry_new(void);
void type_registry_destroy(void);

struct type;
struct object; 

const struct type *type_registry_get_type(
    const struct base_string *type_name
);

struct type *type_registry_create_type(
    const struct base_string *type_name,
    const struct base_string *parent_type_name
);


struct object *object_set(
    struct object *o,
    const struct base_string *key,
    struct object *value
);

struct object *object_get(
    struct object *o,
    const struct base_string *key
);

struct object *object_call(
    struct object *o,
    const struct base_string *key,
    const struct object *args
);

const struct type *object_type(
    struct object *o);

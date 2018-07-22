#pragma once

#include "base_string.h"
#include "object.h"

struct type_registry;

struct type_registry *type_registry_new(void);
void type_registry_destroy(struct type_registry *tr);

struct object *type_registry_get_type(
    struct type_registry *tr,
    const struct base_string *type_name
);

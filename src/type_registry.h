#pragma once

#include "base_string.h"
#include "object.h"



void type_registry_new(void);
void type_registry_destroy(void);

struct object *type_registry_get_type(
    const struct base_string *type_name
);

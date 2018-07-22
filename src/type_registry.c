#include "type_registry.h"

#include <malloc.h>

#include "base_map.h"

struct type_registry {
    struct base_map *types;
};

struct type_registry *type_registry_new(void) {
    struct type_registry *tr = calloc(1, sizeof *tr);
    tr->types = base_map_new();
    return tr;
}

void type_registry_destroy(struct type_registry *tr) {
    base_map_destroy(tr->types);
    free(tr);
}

struct object *type_registry_get_type(
    struct type_registry *tr,
    const struct base_string *type_name
) {
    struct object *type = base_map_get(tr->types, type_name);
    
    if(!type) {
        struct base_string *type_string = base_string_new("type");
        type = object_new(type_string, type_name, tr);
        base_string_destroy(type_string);
        base_map_set(tr->types, type_name, type);
    } 
   
    return type;
}

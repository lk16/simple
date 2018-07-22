#include "type_registry.h"

#include <malloc.h>

#include "base_map.h"

static struct type_registry *registry;

struct type_registry {
    struct base_map *types;
};

void type_registry_new(void) {

    if(registry) {
        return;
    }

    registry = calloc(1, sizeof *registry);
    registry->types = base_map_new();
}

void type_registry_destroy(void) {
    base_map_destroy(registry->types);
    registry = NULL;
}

struct object *type_registry_get_type(
    const struct base_string *type_name
) {
    struct object *type = base_map_get(registry->types, type_name);
    
    if(!type) {
        struct base_string *type_string = base_string_new("type");
        type = object_new(type_string, type_name);
        base_string_destroy(type_string);
        base_map_set(registry->types, type_name, type);
    } 
   
    return type;
}

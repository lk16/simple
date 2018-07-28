#include "type.h"

#include "base_map.h"

#include <malloc.h>
#include <assert.h>


struct type_registry {
    struct base_map *types;
};

enum object_kind {
    OBJECT_NULL,
    OBJECT_BOOL,
    OBJECT_INTEGER,
    OBJECT_DOUBLE,
    OBJECT_STRING,
    OBJECT_VECTOR,
    OBJECT_MAP,
    OBJECT_FUNCTION,
    OBJECT_TYPE,
    OBJECT_STRUCTURE
};

typedef struct object*(*memberfunc_t)(struct object *o, const struct object *args);

struct type {
    struct base_map *attributes;
    struct type *parent;
};

struct object {
    enum object_kind kind;
    bool constant;
    union {
        int value_integer;
        bool value_boolean; 
        double value_double;
        struct base_string *value_string;
        memberfunc_t value_function;
        struct base_map *value_structure;
    };
    struct type *type;
    struct object *parent;
};

static struct object *object_new_type(struct type *type) {
    struct object *o = calloc(1, sizeof *o);
    o->constant = true;
    o->kind = OBJECT_TYPE;
    o->type = type;
    o->parent = NULL;
    return o;    
}

static struct object *object_new_function(memberfunc_t func) {
    struct object *o = calloc(1, sizeof *o);
    o->constant = true;
    o->kind = OBJECT_FUNCTION;
    o->type = NULL; // TODO create function type
    o->value_function = func;
    o->parent = NULL;
    return o;    
}

static struct type *type_new(struct type *parent) {
    struct type *type = malloc(sizeof *type);
    type->attributes = base_map_new();
    type->parent = parent;
    return type;
}

static void type_add_memberfunc(struct type *type, const char *name, memberfunc_t func) {
    struct base_string *key = base_string_new(name);
    struct object *value = object_new_function(func);
    base_map_set(type->attributes, key, value);
    base_string_destroy(key);
}

static struct object *base_memberfunc_not_implemented(
    struct object *o,
    const struct object *args
) {
    (void)o;
    (void)args;
    printf("this is not implemented!\n");
    return o;
}

struct object *object_set(
    struct object *o,
    const struct base_string *key,
    struct object *value
) {
    struct object *ancestor = o;
    
    while(ancestor) {
        if(ancestor->kind != OBJECT_STRUCTURE) {
            break;
        }

        struct object *entry = base_map_get(ancestor->value_structure, key);
        if(entry) {
            base_map_set(ancestor->value_structure, key, value);
            // return null object
            return NULL;
        }
        ancestor = ancestor->parent;
    }

    base_map_set(o->value_structure, key, value);
    // return null object
    return NULL;
}

struct object *object_get(
    struct object *o,
    const struct base_string *key
) {
    while(o) {
        if(o->kind != OBJECT_STRUCTURE) {
            // TODO error
            return NULL;
        }
        struct object *result = base_map_get(o->value_structure, key);
        if(result) {
            return result;
        }
        o = o->parent;
    }
    
    return NULL;
}

struct object *object_call(
    struct object *o,
    const struct base_string *key,
    const struct object *args
) {
    struct object *func = object_get(o, key);
    
    // TODO if not found return type_call()
    
    if(func->kind != OBJECT_FUNCTION) {
        return func;
    }

    return func->value_function(o, args);
}

static struct type *type_base_new() {
    struct type *base = type_new(NULL);
    type_add_memberfunc(base, "_new", base_memberfunc_not_implemented);
    return base;    
}

static struct type_registry *registry;

void type_registry_new(void) {

    if(registry) {
        return;
    }

    registry = calloc(1, sizeof *registry);
    registry->types = base_map_new();
    
    struct base_string *key = base_string_new("base");
    struct object *value = object_new_type(type_base_new());
    base_map_set(registry->types, key, value);
    base_string_destroy(key);
}

void type_registry_destroy(void) {
    base_map_destroy(registry->types);
    registry = NULL;
}

const struct type *type_registry_get_type(
    const struct base_string *type_name
) {
    const struct object *o = base_map_get(registry->types, type_name);
    if(o->kind != OBJECT_TYPE) {
        return NULL;
    }
    return o->type;
}

struct type *type_registry_create_type(
    const struct base_string *type_name,
    const struct base_string *parent_type_name
) {
    struct object *type = base_map_get(registry->types, type_name);
    if(type) {
        // TODO return error
        return NULL;
    }

    struct object *parent = base_map_get(registry->types, parent_type_name);
    if((!parent) || parent->kind != OBJECT_TYPE) {
        // TODO return error
        return NULL;
    }

    struct type *new_type = type_new(parent->type);
    base_map_set(registry->types, type_name, type);
    return new_type;
}


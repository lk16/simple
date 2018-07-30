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
    const char *name;
    struct base_map *attributes;
    struct type *parent;
    bool instantiated;
    enum object_kind instance_kind;
};

struct object {
    enum object_kind kind;
    bool constant;
    union {
        int value_integer;
        bool value_boolean; 
        double value_double;
        struct string*value_string;
        memberfunc_t value_function;
        struct base_map *value_structure;
    };
    const struct type *type;
};

static struct object *object_new(
    enum object_kind kind,
    bool constant,
    const struct type *type
) {
    struct object *o = malloc(sizeof *o);
    o->kind = kind;
    o->constant = constant;
    o->type = type;
    return o;
}

static struct object *object_new_function(
    memberfunc_t func,
    bool constant
) {
    const struct type *func_type = type_registry_get_type("func");
    
    if(!func_type) {
        // TODO return error
        return NULL;
    }
    
    struct object *o = object_new(OBJECT_FUNCTION, constant, func_type);
    o->value_function = func;
    return o;
}

static struct type_registry *registry;

static struct type *type_new(
    const char *type_name
) {
    struct type *type = malloc(sizeof *type);
    type->name = type_name;
    type->instantiated = false;
    type->attributes = base_map_new();
    return type;
}

static void type_destroy(struct type *type) {
    base_map_destroy(type->attributes);
    free(type);
}

static struct object *int_init(struct object *o, const struct object *args) {
    (void)args;
    printf("int init was called!\n");
    return o;
}

static void type_registry_register_builtin_int(){
    struct type *type = type_registry_create_type("int");
    base_map_set(type->attributes, "_init", object_new_function(int_init, true));
}

static void type_registry_register_builtin_func(){
    type_registry_create_type("func");
}

void type_registry_new(void) {

    if(registry) {
        return;
    }

    registry = calloc(1, sizeof *registry);
    registry->types = base_map_new();

    type_registry_register_builtin_func();
    type_registry_register_builtin_int();
}

void type_registry_destroy(void) {
    base_map_destroy(registry->types);
    registry = NULL;
}

const struct type *type_registry_get_type(
    const char *type_name
) {
    const struct object *o = base_map_get(registry->types, type_name);
    if(o->kind != OBJECT_TYPE) {
        return NULL;
    }
    return o->type;
}

struct type *type_registry_create_type(
    const char *type_name
) {
    struct object *type = base_map_get(registry->types, type_name);
    if(type) {
        // TODO return error
        return NULL;
    }

    struct type *new_type = type_new(type_name);
    base_map_set(registry->types, type_name, type);
    return new_type;
}

struct object *type_registry_instantiate(
    const char *type_name
) {
    struct object *type_object = base_map_get(registry->types, type_name);
    if(!type_object) {
        // TODO return error
        return NULL;
    }
    
    if(type_object->kind != OBJECT_TYPE) {
        // TODO return error
        return NULL;
    }
    
    const struct type *type = type_object->type;
    type_object->constant = true;
    
    struct object *o = malloc(sizeof *o);
    o->kind = type->instance_kind;
    o->type = type;
    
    struct object *initializer = base_map_get(type->attributes, "_init");
 
    if(!initializer) {
        return o;
    }
    
    if(initializer->kind != OBJECT_FUNCTION) {
        // TODO return error
        return NULL;
    }
    
    return initializer->value_function(o, NULL);
}

struct object *object_set(
    struct object *o,
    const char *key,
    struct object *value
) {
    if(o->constant) {
        // TODO return error
        return NULL;
    }
    
    if(o->kind == OBJECT_TYPE) {
        // TODO return error
        return NULL;
    }
    
    if(key[0] == '_') {
        // TODO return error
        return NULL;
    }
      
    base_map_set(o->value_structure, key, value);
    // TODO return null object
    return NULL;
}

struct object *object_get(
    struct object *o,
    const char *key
) {
    return base_map_get(o->value_structure, key);
}

struct object *object_call(
    struct object *o,
    const char *key,
    const struct object *args
) {
    struct object *func = object_get(o, key);
    
    // TODO if not found return type_call()
    
    if(func->kind != OBJECT_FUNCTION) {
        return func;
    }

    return func->value_function(o, args);
}


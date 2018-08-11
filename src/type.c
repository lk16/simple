#include "type.h"

#include "base_map.h"
#include "builtin_types.h"

#include <assert.h>
#include <malloc.h>

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
        struct string *value_string;
        memberfunc_t value_function;
        struct base_map *value_structure;
    };
    const struct type *type;
};

static struct object *object_new(enum object_kind kind, bool constant,
                                 const struct type *type)
{
    struct object *o = malloc(sizeof * o);
    o->kind = kind;
    o->constant = constant;
    o->type = type;
    return o;
}

struct object *object_new_function(memberfunc_t func)
{
    const struct type *func_type = type_registry_get_type("func");

    if (!func_type) {
        // TODO return error
        return NULL;
    }

    struct object *o = object_new(OBJECT_FUNCTION, true, func_type);

    o->value_function = func;

    return o;
}

static struct type_registry *registry;

static struct type *type_new(const char *type_name)
{
    struct type *type = malloc(sizeof * type);
    type->name = type_name;
    type->instantiated = false;
    type->attributes = base_map_new();
    return type;
}

static void type_destroy(struct type *type)
{
    base_map_destroy(type->attributes);
    free(type);
}

void type_set_attribute(struct type *type, const char *key,
                        struct object *value)
{
    base_map_set(type->attributes, key, value);
}

void type_registry_new(void)
{

    if (registry) {
        return;
    }

    registry = calloc(1, sizeof * registry);
    registry->types = base_map_new();
    register_builtin_types();
}

void type_registry_destroy(void)
{
    base_map_destroy(registry->types);
    registry = NULL;
}

const struct type *type_registry_get_type(const char *type_name)
{
    const struct object *o = base_map_get(registry->types, type_name);

    if (!o) {
        // TODO return error
        return NULL;
    }

    if (o->kind != OBJECT_TYPE) {
        // TODO return error
        return NULL;
    }

    return o->type;
}

struct type *type_registry_create_type(const char *type_name)
{
    struct object *type = base_map_get(registry->types, type_name);

    if (type) {
        // TODO return error
        return NULL;
    }

    struct type *new_type = type_new(type_name);

    base_map_set(registry->types, type_name, type);

    assert(base_map_get(registry->types, type_name));

    return new_type;
}

struct object *type_registry_construct(const char *type_name)
{
    struct object *type_object = base_map_get(registry->types, type_name);

    if (!type_object) {
        // TODO return error
        return NULL;
    }

    if (type_object->kind != OBJECT_TYPE) {
        // TODO return error
        return NULL;
    }

    const struct type *type = type_object->type;
    type_object->constant = true;

    struct object *o = malloc(sizeof * o);
    o->kind = type->instance_kind;
    o->type = type;

    struct object *initializer = base_map_get(type->attributes, "_init");

    if (!initializer) {
        return o;
    }

    if (initializer->kind != OBJECT_FUNCTION) {
        // TODO return error
        return NULL;
    }

    return initializer->value_function(o, NULL);
}

struct object *object_set_attribute(struct object *o, const char *key,
                                    struct object *value)
{
    if (o->constant) {
        // TODO return error
        return NULL;
    }

    if (o->kind == OBJECT_TYPE) {
        // TODO return error
        return NULL;
    }

    if (key[0] == '_') {
        // TODO return error
        return NULL;
    }

    base_map_set(o->value_structure, key, value);
    // TODO return null object
    return NULL;
}

struct object *object_get_attribute(struct object *o, const char *key)
{
    if (!o) {
        // TODO return error
        return NULL;
    }

    return base_map_get(o->value_structure, key);
}

struct object *object_call(struct object *o, const char *key,
                           const struct object *args)
{

    if (!o) {
        // TODO return error
        return NULL;
    }

    struct object *func = object_get_attribute(o, key);

    if (!func) {

        // TODO if not found return type_call()

        // TODO else return error
        return NULL;
    }

    if (func->kind != OBJECT_FUNCTION) {
        return func;
    }

    return func->value_function(o, args);
}

const struct object *object_get_const_attribute(const struct object *o,
                                                const char *key)
{
    return base_map_get_const(o->value_structure, key);
}

void object_get_double(const struct object *o, bool *ok, double *value)
{
    if (o->kind == OBJECT_DOUBLE) {
        *value = o->value_double;
        *ok = true;
    } else {
        *ok = false;
    }
}

void object_get_int(const struct object *o, bool *ok, int *value)
{
    if (o->kind == OBJECT_INTEGER) {
        *value = o->value_integer;
        *ok = true;
    } else {
        *ok = false;
    }
}

void object_set_int(struct object *o, bool *ok, int value)
{
    if (o->kind == OBJECT_INTEGER) {
        o->value_integer = value;
        *ok = true;
    } else {
        *ok = false;
    }
}

void object_set_double(struct object *o, bool *ok, double value)
{
    if (o->kind == OBJECT_DOUBLE) {
        o->value_double = value;
        *ok = true;
    } else {
        *ok = false;
    }
}

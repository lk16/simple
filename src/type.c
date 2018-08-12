#include "type.h"

#include "simple_types.h"
#include "builtin_types.h"

#include <assert.h>
#include <malloc.h>

struct type_registry {
    struct simple_hashtable *types;
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
    struct simple_string *name;
    struct simple_hashtable *attributes;
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
        struct simple_string *value_string;
        memberfunc_t value_function;
        struct simple_hashtable *value_structure;
        struct type *value_type;
    };
    const struct type *type;
};

size_t object_get_hash(const struct object *o) {
    switch(o->kind) {
        case OBJECT_STRING:
            return simple_string_hash(o->value_string);
        case OBJECT_NULL:
        case OBJECT_BOOL:
        case OBJECT_INTEGER:
        case OBJECT_DOUBLE:
        case OBJECT_VECTOR:
        case OBJECT_MAP:
        case OBJECT_FUNCTION:
        case OBJECT_TYPE:
        case OBJECT_STRUCTURE:
            printf("object_get_hash() not defined for this type!\n");
            return 0;
    }
}

static struct object *object_new(enum object_kind kind, bool constant, const struct type *type) {
    struct object *o = malloc(sizeof * o);
    o->kind = kind;
    o->constant = constant;
    o->type = type;
    return o;
}

struct object *object_new_function(memberfunc_t func) {
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

static struct type *type_new(const char *type_name) {
    struct type *type = malloc(sizeof * type);
    type->name = simple_string_new(type_name);
    type->instantiated = false;

    // TODO
    const struct type *key_type = type_registry_get_type("string");
    const struct type *value_type = type_registry_get_type("object");
    type->attributes = simple_hashtable_new(key_type, value_type);
    return type;
}

static void type_destroy(struct type *type) {
    simple_hashtable_destroy(type->attributes);
    free(type);
}

struct simple_error *type_set_attribute(struct type *type, const char *key, struct object *value) {
    struct object *key_object = object_new_string("%s", key);
    struct simple_error *error = simple_hashtable_upsert(type->attributes, key_object, value, NULL);
    object_destroy(key_object);
    return error;
}

void type_registry_new(void) {

    if (registry) {
        return;
    }

    struct object *string_type = object_new_type("string");
    struct object *string_string = object_new_string("%s", "string");

    struct object *type_type = object_new_type("type");
    struct object *type_string = object_new_string("%s", "type");

    registry = calloc(1, sizeof * registry);
    registry->types = simple_hashtable_new(string_type->value_type, type_type->value_type);

    simple_hashtable_insert(registry->types, string_string, string_type);
    simple_hashtable_insert(registry->types, type_string, type_type);
    register_builtin_types();
}

void type_registry_destroy(void) {
    simple_hashtable_destroy(registry->types);
    registry = NULL;
}

const struct type *type_registry_get_type(const char *type_name) {

    const struct object *o;
    struct object *type_name_object = object_new_string("%s", type_name);
    simple_hashtable_find_const(registry->types, type_name_object, &o);

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

struct simple_error *type_registry_create_type(const char *type_name) {
    struct object *type_name_object = object_new_string("%s", type_name);
    struct object *type_object = object_new_type(type_name);
    struct simple_error *error = simple_hashtable_insert(registry->types, type_name_object, type_object);
    return error;
}

void object_destroy(struct object *o) {
    // TODO
    (void)o;
}

struct simple_error *type_registry_construct(const char *type_name, struct object **result) {
    struct object *type_object;
    struct object *type_name_object = object_new_string("%s", type_name);
    simple_hashtable_find(registry->types, type_name_object, &type_object);

    if (!type_object) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "type '%s' does not exist.", type_name);
    }

    type_object->constant = true;
    struct type *type = type_object->value_type;

    struct object *o = malloc(sizeof *o);

    o->type = type;
    o->kind = type->instance_kind;

    struct object *attribute_name = object_new_string("%s", "_init");
    struct object *initializer;
    simple_hashtable_find(type->attributes, attribute_name, &initializer);

    if (!initializer) {
        *result = o;
        return NULL;
    }

    if (!object_has_type(initializer, "func")) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "Attribute '_init' of type '%s' is not a func.", type_name);
    }

    struct object *init_return_value = initializer->value_function(o, NULL);

    if(init_return_value) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "Attribute '_init' of type '%s' did not return NULL.", type_name);
    }

    *result = o;
    return NULL;
}

struct simple_error *object_set_attribute(struct object *o, const char *key, struct object *value) {

    if (o->constant) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Object is constant.");
    }

    if (key[0] == '_') {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Cannot set attribute starting with '_'.");
    }

    struct object *key_object = object_new_string("%s", key);
    simple_hashtable_upsert(o->value_structure, key_object, value, NULL);
    return NULL;
}

struct simple_error *object_get_attribute(struct object *o, const char *key, struct object **result) {

    if (key[0] == '_') {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Cannot get (non-const) attribute starting with '_'.");
    }

    struct object *key_object = object_new_string("%s", key);
    simple_hashtable_find(o->value_structure, key_object, result);
    return NULL;
}

struct simple_error *object_call(struct object *o, const char *key, const struct object *args, struct object **result) {

    const struct object *function_object;
    struct simple_error *error = object_get_attribute_const(o, key, &function_object);

    if (error) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Failed to call function.");
    }

    if (!function_object) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "Function '%s' does not exist.", key);
    }

    if (function_object->kind != OBJECT_FUNCTION) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "Attribute '%s' is not a function.", key);
    }

    *result = function_object->value_function(o, args);
    return NULL;
}

struct simple_error *object_get_int(const struct object *o, int *result) {
    if(o->kind != OBJECT_INTEGER) {
        *result = 0;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Object is not an int.");
    }
    *result = o->value_integer;
    return NULL;
}

struct simple_error *object_get_double(const struct object *o, double *result) {
    if(o->kind != OBJECT_DOUBLE) {
        *result = 0;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Object is not a double.");
    }
    *result = o->value_double;
    return NULL;
}

struct simple_error *object_set_int(struct object *o, int value) {
    if(o->kind != OBJECT_INTEGER) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Object is not an int.");
    }
    o->value_integer = value;
    return NULL;
}

struct simple_error *object_set_double(struct object *o, double value) {
    if(o->kind != OBJECT_INTEGER) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s", "Object is not an int.");
    }
    o->value_double = value;
    return NULL;
}

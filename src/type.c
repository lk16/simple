#include "type.h"

#include "simple_types.h"
#include "builtin_types.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

struct type_registry {
    struct simple_hashtable *types;
    bool bootstrap;
    struct type *string_type, *type_type, *object_type;
};

enum object_kind {
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

size_t object_get_hash(
    const struct object *o
) {
    switch(o->kind) {
        case OBJECT_STRING:
            return simple_string_hash(o->value_string);
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

static struct object *object_new(
    enum object_kind kind,
    bool constant,
    const struct type *type
) {
    struct object *o = calloc(1, sizeof *o);
    o->kind = kind;
    o->constant = constant;
    o->type = type;
    return o;
}

static struct object *object_new_type(
    const char *type_name,
    enum object_kind instance_kind
) {
    const struct type *type_type = type_registry_get_type("type");
    struct object *o = object_new(OBJECT_TYPE, false, type_type);
    o->value_type = calloc(1, sizeof *o->value_type);
    *o->value_type = (struct type) {
        .name = simple_string_new(type_name),
        .attributes = NULL,
        .instance_kind = instance_kind,
        .instantiated = false
    };
    return o;
}

struct object *object_new_function(
    memberfunc_t func
) {
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

static void type_destroy(
    struct type *type
) {
    simple_hashtable_destroy(type->attributes);
    free(type);
}

struct simple_error *type_set_attribute(
    struct type *type,
    const char *key,
    struct object *value
) {
    struct object *key_object = object_new_string("%s", key);
    if (!type->attributes) {
        type->attributes = simple_hashtable_new(registry->string_type,
            registry->object_type);
    }

    struct simple_error *error = simple_hashtable_insert(type->attributes,
        key_object, value);
    object_destroy(key_object);
    return error;
}

void type_registry_new(
    void
) {
    if (registry) {
        return;
    }

    registry = calloc(1, sizeof *registry);
    *registry = (struct type_registry) {
        .bootstrap = true,
        .types = NULL,
        .type_type = calloc(1, sizeof *registry->type_type),
        .string_type = calloc(1, sizeof *registry->string_type)
    };

    *registry->type_type = (struct type) {
        .name = simple_string_new("type"),
        .attributes = NULL,
        .instantiated = true,
        .instance_kind = OBJECT_TYPE
    };

    *registry->string_type = (struct type) {
        .name = simple_string_new("string"),
        .attributes = NULL,
        .instantiated = true,
        .instance_kind = OBJECT_STRING
    };

    registry->types = simple_hashtable_new(registry->string_type,
        registry->type_type);

    struct object *string_string_object, *type_string_object;
    type_string_object = object_new_string("%s", "type");
    string_string_object = object_new_string("%s", "string");

    struct object *type_type_object, *string_type_object;
    type_type_object = object_new(OBJECT_TYPE, true, registry->type_type);
    string_type_object = object_new(OBJECT_TYPE, true, registry->string_type);

    simple_hashtable_insert(registry->types, type_string_object,
        type_type_object);
    simple_hashtable_insert(registry->types, string_string_object,
        string_type_object);

    registry->bootstrap = false;

    type_registry_create_type("object", &registry->object_type);

    struct type *func_type;
    type_registry_create_type("func", &func_type);

    register_builtin_types();
}

void type_registry_destroy(
    void
) {
    simple_hashtable_destroy(registry->types);
    free(registry->types);
    free(registry);
    registry = NULL;
}

const struct type *type_registry_get_type(
    const char *type_name
) {
    if (registry->bootstrap) {
        if (strcmp(type_name, "string") == 0) {
            return registry->string_type;
        }
        if (strcmp(type_name, "type") == 0) {
            return registry->type_type;
        }
        // TODO return error
        return NULL;
    }

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

struct simple_error *type_registry_create_type(
    const char *type_name,
    struct type **result
) {
    struct object *type_name_object = object_new_string("%s", type_name);

    enum object_kind instance_kind;
    if(strcmp(type_name, "bool") == 0) { instance_kind = OBJECT_BOOL; }
    else if(strcmp(type_name, "int") == 0) { instance_kind = OBJECT_INTEGER; }
    else if(strcmp(type_name, "double") == 0) { instance_kind = OBJECT_DOUBLE; }
    else if(strcmp(type_name, "string") == 0) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "%s", "Cannot override type 'string'");
    }
    else if(strcmp(type_name, "vector") == 0) { instance_kind = OBJECT_VECTOR; }
    else if(strcmp(type_name, "map") == 0) { instance_kind = OBJECT_MAP; }
    else if(strcmp(type_name, "func") == 0) { instance_kind = OBJECT_FUNCTION; }
    else if(strcmp(type_name, "type") == 0) { instance_kind = OBJECT_TYPE; }
    else { instance_kind = OBJECT_STRUCTURE; }

    struct object *type_object = object_new_type(type_name, instance_kind);
    struct simple_error *error = simple_hashtable_insert(registry->types,
        type_name_object, type_object);

    *result = type_object->value_type;
    return error;
}

void object_destroy(
    struct object *o
) {
    printf("%s", "object_destroy() is not implemented()\n");
    (void)o;
}

struct simple_error *type_registry_construct(
    const char *type_name,
    struct object **result
) {
    struct object *type_object;
    struct object *type_name_object = object_new_string("%s", type_name);
    simple_hashtable_find(registry->types, type_name_object, &type_object);

    if (!type_object) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "type '%s' does not exist.", type_name);
    }

    type_object->constant = true;
    struct type *type = type_object->value_type;

    struct object *o = calloc(1, sizeof *o);

    o->type = type;
    o->kind = type->instance_kind;

    struct object *attribute_name = object_new_string("%s", "_init");
    struct object *initializer;
    simple_hashtable_find(type->attributes, attribute_name, &initializer);

    if (!initializer) {
        *result = o;
        return NULL;
    }

    struct object *init_return_value;
    struct simple_error *child_error = object_call(o, "_init", NULL,
        &init_return_value);

    if(child_error) {
        *result = NULL;
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "Constructing instance of type '%s' failed.",
            type_name);
        simple_error_set_child(error, child_error);
        return error;
    }

    if(init_return_value) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "Attribute '_init' of type '%s' did not return NULL.", type_name);
    }

    *result = o;
    return NULL;
}

struct simple_error *object_set_attribute(
    struct object *o,
    const char *key,
    struct object *value
) {
    if (o->constant) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is constant.");
    }

    if (key[0] == '_') {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Cannot set attribute starting with '_'.");
    }

    if (!o->value_structure) {
        o->value_structure = simple_hashtable_new(registry->string_type,
            registry->object_type);
    }

    struct object *key_object = object_new_string("%s", key);
    simple_hashtable_insert(o->value_structure, key_object, value);
    return NULL;
}

struct simple_error *object_get_attribute(
    struct object *o,
    const char *key,
    struct object **result
) {

    if (key[0] == '_') {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
        "Cannot get (non-const) attribute starting with '_'.");
    }

    struct object *key_object = object_new_string("%s", key);
    simple_hashtable_find(o->value_structure, key_object, result);
    return NULL;
}

struct simple_error *object_get_attribute_const(
    const struct object *o,
    const char *key,
    const struct object **result
) {
    struct object *key_object = object_new_string("%s", key);
    simple_hashtable_find_const(o->value_structure, key_object, result);
    return NULL;
}

struct simple_error *object_call(
    struct object *o,
    const char *key,
    const struct object *args,
    struct object **result
) {
    const struct object *attribute;
    struct simple_error *error = object_get_attribute_const(o, key, &attribute);
    if (error) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Failed to call function.");
    }

    if (!attribute) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "Function '%s' does not exist.", key);
    }

    if (attribute->kind != OBJECT_FUNCTION) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "Attribute '%s' is not a function.", key);
    }

    struct simple_error *child_error = attribute->value_function(o, args,
        result);

    if (child_error) {
        error = simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "Error in function %s.%s()", o->type->name, key);
        simple_error_set_child(error, child_error);
        *result = NULL;
        return error;
    }

    return NULL;
}

struct simple_error *object_get_int(
    const struct object *o,
    int *result
) {
    if(o->kind != OBJECT_INTEGER) {
        *result = 0;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is not an int.");
    }
    *result = o->value_integer;
    return NULL;
}

struct simple_error *object_get_string(
    const struct object *o,
    const struct simple_string **result
) {
    if(o->kind != OBJECT_STRING) {
        *result = 0;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is not a string.");
    }
    *result = o->value_string;
    return NULL;
}

struct simple_error *object_get_double(
    const struct object *o,
    double *result
) {
    if(o->kind != OBJECT_DOUBLE) {
        *result = 0;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
        "Object is not a double.");
    }
    *result = o->value_double;
    return NULL;
}

struct simple_error *object_set_int(
    struct object *o,
    int value
) {
    if(o->kind != OBJECT_INTEGER) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is not an int.");
    }
    o->value_integer = value;
    return NULL;
}

struct simple_error *object_set_double(
    struct object *o,
    double value
) {
    if(o->kind != OBJECT_INTEGER) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is not an int.");
    }
    o->value_double = value;
    return NULL;
}

struct object *object_new_string(
    const char *format,
    ...
) {
    struct object *o = object_new(OBJECT_STRING, true, registry->string_type);

    char *buff = NULL;
    va_list args, args2;
    va_start(args, format);
    size_t length = (size_t)vsnprintf(buff, 0, format, args) + 1;
    va_end (args);

    buff = calloc(length, sizeof *buff);

    va_start(args2, format);
    vsnprintf(buff, length, format, args2);
    va_end (args2);

    o->value_string = simple_string_new(buff);
    return o;
}

struct object *object_copy(
    const struct object *o
) {
    switch(o->kind) {
        case OBJECT_BOOL:
        case OBJECT_INTEGER:
        case OBJECT_TYPE:
        case OBJECT_FUNCTION:
        case OBJECT_DOUBLE: {
            struct object *copy = calloc(1, sizeof *copy);
            memcpy(copy, o, sizeof *copy);
            copy->constant = false;
            return copy;
        }
        case OBJECT_STRING: {
            struct object *copy = calloc(1, sizeof *copy);
            memcpy(copy, o, sizeof *copy);
            copy->value_string = simple_string_copy(o->value_string);
            copy->constant = false;
            return copy;
        }
        case OBJECT_VECTOR:
        case OBJECT_MAP:
        case OBJECT_STRUCTURE: {
            printf("Copying for this object_kind is not implemented\n");
            return NULL;
        }
    }
}

bool object_has_type(
    const struct object *o,
    const struct type *type
) {
    if(strcmp(simple_string_get(type->name), "object") == 0) {
        return true;
    }

    return o->type == type;
}

bool object_equals(
    const struct object *lhs,
    const struct object *rhs
) {
    if(lhs->type != rhs->type) {
        return false;
    }

    switch(lhs->kind) {
        case OBJECT_BOOL:
            return lhs->value_boolean == rhs->value_boolean;
        case OBJECT_INTEGER:
            return lhs->value_integer == rhs->value_integer;
        case OBJECT_DOUBLE:
            // silence compiler warning
            return lhs->value_double >= rhs->value_double &&
                lhs->value_double <= rhs->value_double;
        case OBJECT_STRING:
            return simple_string_equals(lhs->value_string, rhs->value_string);
        case OBJECT_VECTOR:
        case OBJECT_MAP:
        case OBJECT_FUNCTION:
        case OBJECT_TYPE:
        case OBJECT_STRUCTURE: {
            printf("Equality check for this object_kind is not implemented\n");
            return false;
        }
    }
}

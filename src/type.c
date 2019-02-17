#include "type.h"

#include "simple_error.h"
#include "simple_hashtable.h"
#include "simple_object.h"
#include "simple_string.h"
#include "builtin_types.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

struct type_registry {
    struct simple_hashtable *types;
    bool bootstrap;
    struct type *string_type, *type_type, *object_type;
};

struct type_registry *registry;

struct type {
    struct simple_string *name;
    struct simple_hashtable *attributes;
    bool instantiated;
    enum object_kind instance_kind;
};

struct simple_error *type_registry_get_string_type(
    struct type **result
) {
    *result = registry->string_type;
    return NULL;
}



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
    struct simple_error *error;
    struct object *key_object = NULL;
    error = object_new_string(&key_object, "%s", key);
    simple_error_check(error);

    if (!type->attributes) {
        type->attributes = simple_hashtable_new(registry->string_type,
            registry->object_type);
    }

    error = simple_hashtable_insert(type->attributes, key_object, value);
    simple_error_check(error);

    cleanup:
    object_refcount_decrease(key_object);
    return error;
}

struct simple_error *type_registry_new(
    void
) {
    if (registry) {
        return NULL;
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

    struct simple_error *error = NULL;
    struct object *type_string_object = NULL;
    struct object *string_string_object = NULL;
    struct object *string_type_object = NULL;
    struct object *type_type_object = NULL;

    error = object_new_string(&type_string_object, "%s", "type");
    simple_error_check(error);

    error = object_new_string(&string_string_object, "%s", "string");
    simple_error_check(error);

    error = object_new_type("type", OBJECT_TYPE, &type_type_object);
    simple_error_check(error);

    error = object_get_type(type_type_object, &registry->type_type);
    simple_error_check(error);

    error = object_new_type("string", OBJECT_STRING, &string_type_object);
    simple_error_check(error);

    error = object_get_type(string_type_object, &registry->string_type);
    simple_error_check(error);

    error = simple_hashtable_insert(registry->types, type_string_object,
        type_type_object);
    simple_error_check(error);

    error = simple_hashtable_insert(registry->types, string_string_object,
        string_type_object);
    simple_error_check(error);

    registry->bootstrap = false;

    error = type_registry_create_type("object", &registry->object_type);
    simple_error_check(error);

    struct type *func_type = NULL;
    error = type_registry_create_type("func", &func_type);
    simple_error_check(error);

    error = register_builtin_types();
    simple_error_check(error);

    cleanup:
    object_refcount_decrease(type_string_object);
    object_refcount_decrease(string_string_object);
    object_refcount_decrease(type_type_object);
    object_refcount_decrease(string_type_object);

    if (error) {
        (void)type_registry_destroy();
    }

    return error;
}

struct simple_error *type_registry_destroy(
    void
) {
    simple_hashtable_destroy(registry->types);
    free(registry);
    registry = NULL;
    return NULL;
}

struct simple_error *type_registry_get_type(
    const char *type_name,
    struct type **result
) {
    if (registry->bootstrap) {
        if (strcmp(type_name, "string") == 0) {
            *result = registry->string_type;
            return NULL;
        }
        if (strcmp(type_name, "type") == 0) {
            *result = registry->type_type;
            return NULL;
        }
        *result = NULL;
        return simple_error_new(
            "Cannot get type '%s' when bootstrapping type system.", type_name);
    }

    struct object *o = NULL;
    struct object *type_name_object;
    struct simple_error *error;

    error = object_new_string(&type_name_object, "%s", type_name);
    simple_error_check(error);

    error = simple_hashtable_find(registry->types, type_name_object, &o);
    simple_error_check(error);

    if (!o) {
        error = simple_error_new("Type '%s' does not exist.", type_name);
        simple_error_check(error);
    }

    error = object_get_type(o, result);
    simple_error_check(error);

    cleanup:
    if (error) {
        object_refcount_decrease(o);
    }

    return error;
}

struct simple_error *type_registry_create_type(
    const char *type_name,
    struct type **result
) {
    struct simple_error *error = NULL;
    struct object *type_name_object = NULL;
    struct object *type_object = NULL;

    // assign 0 to silence compiler warning
    enum object_kind instance_kind = 0;

    if (strcmp(type_name, "int") == 0) {
        instance_kind = OBJECT_INTEGER;
    } else if (strcmp(type_name, "string") == 0) {
        *result = NULL;
        return simple_error_new("%s", "Cannot override type 'string'");
    } else if (strcmp(type_name, "func") == 0) {
        instance_kind = OBJECT_FUNCTION;
    } else if (strcmp(type_name, "type") == 0) {
        instance_kind = OBJECT_TYPE;
    }else if (strcmp(type_name, "object") == 0) {
        instance_kind = OBJECT_STRING;
    } else {
        error = simple_error_new("Cannot create type '%s', associated "
            "instance_kind unknown", type_name);
        simple_error_check(error);
    }

    error = object_new_string(&type_name_object, "%s", type_name);
    simple_error_check(error);

    error = object_new_type(type_name, instance_kind, &type_object);
    simple_error_check(error);

    error = simple_hashtable_insert(registry->types, type_name_object,
        type_object);
    simple_error_check(error);

    error = object_get_type(type_object, result);
    simple_error_check(error);

    cleanup:
    object_refcount_decrease(type_name_object);
    object_refcount_decrease(type_object);

    if (error) {
        *result = NULL;
    }

    return error;
}

struct simple_error *type_registry_construct(
    const char *type_name,
    struct object **result
) {
    struct object *type_object, *type_name_object;
    struct simple_error *error;

    error = object_new_string(&type_name_object, "%s", type_name);
    simple_error_check(error);

    error = simple_hashtable_find(registry->types, type_name_object,
        &type_object);
    simple_error_check(error);

    if (!type_object) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "type '%s' does not exist.", type_name);
    }


    struct type *type;
    error = object_get_type(type_object, &type);
    simple_error_check(error);

    *result = object_new(type->instance_kind, false, type);

    cleanup:
    if (error) {
        *result = NULL;
    }
    return NULL;
}

struct simple_error *type_new(
    const char *type_name,
    enum object_kind instance_kind,
    struct type **result
) {
    struct type *type = calloc(1, sizeof *type);
    *type = (struct type) {
        .name = simple_string_new(type_name),
        .attributes = simple_hashtable_new(registry->string_type,
            registry->type_type),
        .instance_kind = instance_kind,
        .instantiated = false
    };
    *result = type;
    return NULL;
}

struct simple_error *type_get_name(
    const struct type *type,
    const char **result
) {

    *result = simple_string_get(type->name);
    return NULL;
}

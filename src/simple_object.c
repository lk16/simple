#include <malloc.h>
#include <string.h>

#include "simple_error.h"
#include "simple_object.h"
#include "simple_string.h"
#include "type.h"

const char *object_kind_get_name(enum object_kind kind) {
    switch (kind) {
        case OBJECT_INTEGER:
            return "integer";
        case OBJECT_STRING:
            return "string";
        case OBJECT_FUNCTION:
            return "function";
        case OBJECT_TYPE:
            return "type";
    }
}

struct object {
    enum object_kind kind;
    bool constant;
    int ref_count;
    union {
        int value_integer;
        struct simple_string *value_string;
        memberfunc_t value_function;
        struct type *value_type;
    };
    const struct type *type;
};

size_t object_get_hash(
    const struct object *o
) {
    switch (o->kind) {
        case OBJECT_STRING:
            return simple_string_hash(o->value_string);
        case OBJECT_INTEGER:
        case OBJECT_FUNCTION:
        case OBJECT_TYPE:
            printf("object_get_hash() not defined for this type!\n");
            return 0;
    }
}

struct object *object_new(
    enum object_kind kind,
    bool constant,
    const struct type *type
) {
    struct object *o = calloc(1, sizeof *o);
    o->kind = kind;
    o->constant = constant;
    o->type = type;
    o->ref_count = 1;
    return o;
}

struct simple_error *object_new_type(
    const char *type_name,
    enum object_kind instance_kind,
    struct object **result
) {
    struct simple_error *error;
    struct type *type_type = NULL;
    struct object *o = NULL;

    error = type_registry_get_type("type", &type_type);
    simple_error_check(error);

    o = object_new(OBJECT_TYPE, false, type_type);
    error = type_new(type_name, instance_kind, &o->value_type);
    simple_error_check(error);

    *result = o;

    cleanup:
    if (error) {
        object_refcount_decrease(o);
    }
    return error;
}

struct simple_error *object_new_function(
    memberfunc_t func,
    struct object **result
) {
    struct simple_error *error = type_registry_construct("func", result);
    simple_error_check(error);

    (*result)->value_function = func;

    cleanup:
    if (error) {
        object_refcount_decrease(*result);
    }
    return NULL;
}


struct simple_error *object_get_int(
    const struct object *o,
    int *result
) {
    if (o->kind != OBJECT_INTEGER) {
        *result = 0;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is not an int.");
    }
    *result = o->value_integer;
    return NULL;
}

struct simple_error *object_get_string(
    struct object *o,
    struct simple_string **result
) {
    if (o->kind != OBJECT_STRING) {
        *result = NULL;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is not a string.");
    }
    *result = o->value_string;
    return NULL;
}

struct simple_error *object_set_int(
    struct object *o,
    int value
) {
    if (o->kind != OBJECT_INTEGER) {
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Object is not an int.");
    }
    o->value_integer = value;
    return NULL;
}

struct simple_error *object_new_string(
    struct object **result,
    const char *format,
    ...
) {
    struct simple_error *error = NULL;
    struct type *string_type;

    error = type_registry_get_string_type(&string_type);
    simple_error_check(error);

    struct object *o = object_new(OBJECT_STRING, true, string_type);

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
    free(buff);

    *result = o;

    cleanup:
    if (error) {
        *result = NULL;
    }
    return error;
}

struct simple_error *object_copy(
    const struct object *o,
    struct object **copy
) {
    switch (o->kind) {
        case OBJECT_INTEGER:
        case OBJECT_TYPE:
        case OBJECT_FUNCTION:
        case OBJECT_STRING: {
            *copy = calloc(1, sizeof **copy);
            memcpy(*copy, o, sizeof **copy);
            (*copy)->value_string = simple_string_copy(o->value_string);
            (*copy)->constant = false;
            return NULL;
        }
    }
}

struct simple_error *object_has_type(
    const struct object *o,
    const struct type *type,
    bool *result
) {
    struct simple_error *error = NULL;
    const char *type_name;

    error = type_get_name(type, &type_name);
    simple_error_check(error);

    if (strcmp(type_name, "object") == 0) {
        *result = true;
    }
    else {
        *result = (o->type == type);
    }

    cleanup:
    if (error) {
        *result = false;
    }
    return error;
}

struct simple_error *object_check_type(
    const struct object *o,
    const struct type *type
) {
    bool has_type;
    struct simple_error *error = object_has_type(o, type, &has_type);
    simple_error_check(error);

    const char *expected_type_name;
    error = type_get_name(type, &expected_type_name);
    simple_error_check(error);

    const char *object_type_name;

    error = type_get_name(o->type, &object_type_name);
    simple_error_check(error);


    if (!has_type) {
        error = simple_error_new("Invalid object type, expected %s, got %s",
            expected_type_name, object_type_name);
    }

    cleanup:
    return error;
}

struct simple_error *object_equals(
    const struct object *lhs,
    const struct object *rhs,
    bool *result
) {
    struct simple_error *error = NULL;
    const char *lhs_type_name;

    error = type_get_name(lhs->type, &lhs_type_name);
    simple_error_check(error);

    if (lhs->type != rhs->type) {

        const char *rhs_type_name;
        error = type_get_name(rhs->type, &rhs_type_name);
        simple_error_check(error);

        error = simple_error_new(__FILE__, __LINE__, __FUNCTION__,
            "Cannot check objects with different types '%s' and '%s' "
            "for equality", lhs_type_name, rhs_type_name);
        simple_error_check(error);
    }

    switch (lhs->kind) {
        case OBJECT_INTEGER:
            *result = lhs->value_integer == rhs->value_integer;
            break;
        case OBJECT_STRING:
            *result = simple_string_equals(lhs->value_string,
                rhs->value_string);
            break;
        case OBJECT_FUNCTION:
        case OBJECT_TYPE: {
            error = simple_error_new(__FILE__, __LINE__, __FUNCTION__,
                "object_equals() is not implemented for type '%s'.",
                lhs_type_name);
        }
    }

    cleanup:
    if (error) {
        *result = false;
    }
    return error;
}

struct simple_error *object_get_type(
    const struct object *o,
    struct type **result
) {
    if (o->kind != OBJECT_TYPE) {
        *result = NULL;
        return simple_error_new("Object is not a type but %s",
            object_kind_get_name(o->kind));
    }
    *result = o->value_type;
    return NULL;
}

void object_refcount_decrease(
    struct object *o
) {
    if (!o) {
        return;
    }
    o->ref_count--;
}

void object_refcount_increase(
    struct object *o
) {
    if (!o) {
        return;
    }
    o->ref_count++;
}

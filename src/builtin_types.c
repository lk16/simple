#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "builtin_types.h"
#include "simple_types.h"
#include "type.h"

static struct simple_error *int_assign(
    struct object *o,
    const struct object *args,
    struct object **result
) {
    printf("int assign was called!\n");

    int value;
    struct simple_error *child_error = object_get_int(args, &value);

    if (child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s", "Getting int value failed.");
        simple_error_set_child(error, child_error);
        *result = NULL;
        return error;
    }

    child_error = object_set_int(o, value);

    if (child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s", "Setting int value failed.");
        simple_error_set_child(error, child_error);
        *result = NULL;
        return error;
    }

    *result = o;
    return NULL;
}

static struct simple_error *int_init(
    struct object *o,
    const struct object *args,
    struct object **result
) {
    printf("int init was called!\n");

    struct simple_error *child_error;
    if (args) {
        child_error = int_assign(o, args, result);
    } else {
        child_error = object_set_int(o, 0);
    }

    if (child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s", "Assigning int failed.");
        simple_error_set_child(error, child_error);
        *result = NULL;
        return error;
    }

    return NULL;
}

static struct simple_error *int_print(
    struct object *o,
    const struct object *args,
    struct object **result
) {
    (void)args;

    printf("int init was called!\n");

    int value;
    struct simple_error *child_error = object_get_int(o, &value);

    if (child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s", "Getting int value failed.");
        simple_error_set_child(error, child_error);
        *result = NULL;
        return error;
    }

    printf("%d\n", value);

    *result = NULL;
    return NULL;
}

void register_builtin_types(
    void
) {

    struct simple_error *error;
    struct type *int_type, *func_type;
    struct object *function = NULL;

    error = type_registry_create_type("int", &int_type);

    (error = object_new_function(int_init, &function)) ||
    (error = type_set_attribute(int_type, "_init", function)) ||

    (error = object_new_function(int_assign, &function)) ||
    (error = type_set_attribute(int_type, "_assign", function)) ||

    (error = object_new_function(int_print, &function)) ||
    (error = type_set_attribute(int_type, "_print", function)) ||

    (error = type_registry_create_type("func", &func_type));

    if (error) {
        simple_error_show(error, stderr);
        simple_error_destroy(error);
    }
}

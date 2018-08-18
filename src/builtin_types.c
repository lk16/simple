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
    struct simple_error *error;

    error = object_get_int(args, &value);
    simple_error_forward(error, "%s", "Getting int value failed.");

    error = object_set_int(o, value);
    simple_error_forward(error, "%s", "Setting int value failed.")

    *result = o;
    return NULL;
}

static struct simple_error *int_init(
    struct object *o,
    const struct object *args,
    struct object **result
) {
    printf("int init was called!\n");

    struct simple_error *error;
    if (args) {
        error = int_assign(o, args, result);
    } else {
        error = object_set_int(o, 0);
    }
    simple_error_forward(error, "%s", "Assigning int failed.");

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
    struct simple_error *error = object_get_int(o, &value);
    simple_error_forward(error, "%s", "Getting int value failed.");

    printf("%d\n", value);

    *result = NULL;
    return NULL;
}

struct simple_error *register_builtin_types(
    void
) {

    struct simple_error *error;
    struct type *int_type, *func_type;
    struct object *function = NULL;

    error = type_registry_create_type("int", &int_type);
    simple_error_forward(error, "%s", "");

    error = object_new_function(int_init, &function);
    simple_error_forward(error, "%s", "");

    error = type_set_attribute(int_type, "_init", function);
    simple_error_forward(error, "%s", "");

    error = object_new_function(int_assign, &function);
    simple_error_forward(error, "%s", "");

    error = type_set_attribute(int_type, "_assign", function);
    simple_error_forward(error, "%s", "");

    error = object_new_function(int_print, &function);
    simple_error_forward(error, "%s", "");

    error = type_set_attribute(int_type, "_print", function);
    simple_error_forward(error, "%s", "");

    error = type_registry_create_type("func", &func_type);
    simple_error_forward(error, "%s", "");

    if (error) {
        simple_error_show(error, stderr);
        simple_error_destroy(error);
    }
    return NULL;
}

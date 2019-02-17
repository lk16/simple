#include "builtin_types.h"

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "simple_error.h"
#include "simple_object.h"
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
    simple_error_check(error);

    error = object_set_int(o, value);
    simple_error_check(error);

    *result = o;
    
    cleanup:
    return error;
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
        simple_error_check(error);
    } else {
        error = object_set_int(o, 0);
        simple_error_check(error);
    }

    cleanup:
    return error;
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
    simple_error_check(error);

    printf("%d\n", value);

    *result = NULL;
    
    cleanup:
    return error;
}

struct simple_error *register_builtin_types(
    void
) {

    struct simple_error *error;
    struct type *int_type, *func_type;
    struct object *function = NULL;
    
    
    error = type_registry_create_type("int", &int_type);
    simple_error_check(error);

    error = object_new_function(int_init, &function);
    simple_error_check(error);

    error = type_set_attribute(int_type, "_init", function);
    simple_error_check(error);

    error = object_new_function(int_assign, &function);
    simple_error_check(error);

    error = type_set_attribute(int_type, "_assign", function);
    simple_error_check(error);

    error = object_new_function(int_print, &function);
    simple_error_check(error);

    error = type_set_attribute(int_type, "_print", function);
    simple_error_check(error);

    error = type_registry_create_type("func", &func_type);
    simple_error_check(error);

    cleanup:
        object_refcount_decrease(function);

    return error;
}

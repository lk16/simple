#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "builtin_types.h"
#include "base_map.h"
#include "type.h"

static struct object *int_assign(struct object *o, const struct object *args)
{

    printf("int assign was called!\n");

    bool ok;
    int value;
    object_get_int(args, &ok, &value);

    if (!ok) {
        // TODO return error
        return NULL;
    }

    object_set_int(o, &ok, value);

    if (!ok) {
        // TODO return error
        return NULL;
    }

    return o;
}

static struct object *int_init(struct object *o, const struct object *args)
{

    printf("int init was called!\n");

    if (args) {
        o = int_assign(o, args);
    } else {
        bool ok;
        object_set_int(o, &ok, 0);
    }

    return o;
}

static struct object *int_print(struct object *o, const struct object *args)
{

    printf("int init was called!\n");

    (void)args;
    int value;
    bool ok;

    object_get_int(o, &ok, &value);

    if (!ok) {
        // TODO return error
        return NULL;
    }

    printf("%d\n", value);

    return o;
}

void register_builtin_types()
{

    struct type *int_type = type_registry_create_type("int");
    type_set_attribute(int_type, "_init", object_new_function(int_init));
    type_set_attribute(int_type, "_assign", object_new_function(int_assign));
    type_set_attribute(int_type, "_print", object_new_function(int_print));

    type_registry_create_type("func");

}

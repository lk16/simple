#include <stdio.h>

#include "simple_types.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    type_registry_new();

    struct object *o;
    struct simple_error *error;
    const struct simple_string *string;

    object_new_string(&o, "%s, %s!\n", "Hello", "world");
    error = object_get_string(o, &string);
    simple_error_show(error, stdout);
    printf("%s", simple_string_get(string));

    return 0;
}

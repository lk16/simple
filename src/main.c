#include <stdio.h>

#include "simple_types.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    struct object *o;
    struct simple_error *error;
    const struct simple_string *string;

    error = type_registry_new();
    if (error) {
        simple_error_show(error, stdout);
        simple_error_destroy(error);
    }

    error = object_new_string(&o, "%s, %s!\n", "Hello", "world");
    if (error) {
        simple_error_show(error, stdout);
        simple_error_destroy(error);
    }

    error = object_get_string(o, &string);
    if (error) {
        simple_error_show(error, stdout);
        simple_error_destroy(error);
    }

    printf("%s", simple_string_get(string));

    return 0;
}

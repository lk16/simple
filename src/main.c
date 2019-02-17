#include <stdio.h>

#include "simple_error.h"
#include "simple_string.h"
#include "type.h"


int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    struct simple_error *error;

    error = type_registry_new();
    if (error) {
        simple_error_show(error, stdout);
        simple_error_destroy(error);
        return 1;
    }
    
    error = type_registry_destroy();
    if (error) {
        simple_error_show(error, stdout);
        simple_error_destroy(error);
        return 1;
    }

    return 0;
}

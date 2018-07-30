#include <stdio.h>

#include "type.h"


int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    type_registry_new();
    type_registry_destroy();
}


#include <stdio.h>

#include "type.h"


int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    type_registry_new();
    
    // should print 0
    struct object *my_int = type_registry_instantiate("int");
    object_call(my_int, "_print", NULL);
    
    struct object *five = object_new_int(5);
    object_call(my_int, "_init", five);
    object_call(my_int, "_print", NULL);
    
    type_registry_destroy();
}


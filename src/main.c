#include <stdio.h>

#include "base_map.h"
#include "object.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    char key_raw[10];

    struct base_map *map = base_map_new();

    for(int i=0; i<99999; i++) {
        snprintf(key_raw, sizeof(key_raw), "%d", i);
        struct base_string *key = base_string_new(key_raw);
        struct object *value = object_new(i);
        base_map_set(map, key, value);
        base_string_destroy(key);
    }

    const struct base_string *key;
    struct object *value;
    struct base_map_iterator *iter = base_map_iterator_new(map, &key, &value);


    while(base_map_iterator_next(iter)) {
        printf("%s -> %d\n", base_string_raw(key), object_get(value));
    }

    base_map_iterator_destroy(iter);

    base_map_destroy(map);

    return 0;
}

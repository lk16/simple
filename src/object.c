#include "object.h"

#include <malloc.h>

struct object{
    int value;
};

struct object *object_new(int value) {
    struct object *o = calloc(1, sizeof *o);
    o->value = value;
    return o;
}

int object_get(const struct object *o) {
    return o->value;
}

void object_destroy(struct object *o) {
    free(o);
}

#include <malloc.h>

#include "state.h"

struct state *state_new() {
    struct state *s = malloc(sizeof *s);
    s->next_id = 0;
    s->capacity = 20;
    s->objects = malloc(20 * sizeof(void*));
    return s;
}

void state_destroy(struct state *s) {
    free(s->objects);
    free(s);
}

int state_add_object(struct state *s, struct object *t) {

    // TODO check if object is already registered

    int id = s->next_id;

    if(s->capacity == id) {
        size_t new_capacity = s->capacity * 2;
        s->objects = realloc(s->objects, new_capacity);
        s->capacity = new_capacity;
    }

    s->objects[id] = t;
    s->next_id++;
    return id;
}

struct object *state_get_object(struct state *s, char *name) {
    (void)s;
    (void)name;
    return NULL;
}

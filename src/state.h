
#pragma once

#include <stdio.h>

struct object;

struct state {
    struct object **objects;
    int next_id;
    int capacity;
};

struct state *state_new();
void state_destroy(struct state *r);

int state_add_object(struct state *r, struct object *o);
struct object *state_get_object(struct state *r, char *name);


struct object {
    int id;
    char *name;
    struct object *type;
    struct object *parent;
    void(*to_string)(struct object *o, FILE *file);
};


struct object *object_new(struct state *r, struct object *parent_object);

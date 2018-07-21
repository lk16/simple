#pragma once

struct object;

struct object *object_new(int value);
void object_destroy(struct object *o);

int object_get(const struct object *o);

#pragma once

#include <stdbool.h>

struct type;
struct object;
struct string;

typedef struct object *(*memberfunc_t)(struct object *o, const struct object *args);

              void type_registry_new(void);
              void type_registry_destroy(void);
const struct type *type_registry_get_type(const char *type_name);
      struct type *type_registry_create_type(const char *type_name);
    struct object *type_registry_construct(const char *type_name);

void type_set_attribute(struct type *type, const char *key, struct object *value);


      struct object *object_new_int(int value);
      struct object *object_new_double(float value);
      struct object *object_new_function(memberfunc_t value);
      struct object *object_new_string(const struct string *value);

                void object_get_int(const struct object *o, bool *ok, int *value);
                void object_get_double(const struct object *o, bool *ok, double *value);

                void object_set_int(struct object *o, bool *ok, int value);
                void object_set_double(struct object *o, bool *ok, double value);

      struct object *object_set_attribute(struct object *o, const char *key, struct object *value);
      struct object *object_get_attribute(struct object *o, const char *key);
const struct object *object_get_const_attribute(const struct object *o, const char *key);
      struct object *object_call(struct object *o, const char *key, const struct object *args);
  const struct type *object_get_type(struct object *o);

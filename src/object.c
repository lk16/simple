#include "object.h"

#include "base_map.h"

#include <malloc.h>

enum object_kind {
    OBJECT_NULL,
    OBJECT_BOOL,
    OBJECT_INTEGER,
    OBJECT_DOUBLE,
    OBJECT_STRING,
    OBJECT_VECTOR,
    OBJECT_MAP,
    OBJECT_FUNCTION,
    OBJECT_TYPE,
    OBJECT_STRUCTURE
};

typedef struct object*(*object_function_t)(struct object *o, const struct object *args);

struct structure {
    struct base_map *attributes;
    struct structure *parent_structure;
};

struct object {
    enum object_kind kind;
    union {
        int value_integer;
        bool value_boolean; 
        double value_float;
        struct base_string *value_string;
        object_function_t value_function;
        struct type *value_type;
        struct structure *value_structure;
    };
};
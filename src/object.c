#include "object.h"

#include "base_map.h"
#include "type_registry.h"


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

struct object {
    enum object_kind kind;
    bool constant;
    union {
        int value_integer;
        bool value_boolean; 
        double value_float;
        struct base_string *value_string;
        object_function_t value_function;
        struct object *value_type;
        struct structure *value_structure;
    };
    struct base_map *attributes;
    struct object *parent;
};

static bool is_builtin_type(const struct base_string *type_name) {
    return base_string_equals_raw(type_name, "bool")
        || base_string_equals_raw(type_name, "double")
        || base_string_equals_raw(type_name, "function")
        || base_string_equals_raw(type_name, "int")
        || base_string_equals_raw(type_name, "map")
        || base_string_equals_raw(type_name, "null")
        || base_string_equals_raw(type_name, "string")
        || base_string_equals_raw(type_name, "type")
        || base_string_equals_raw(type_name, "vector");
}

static struct object *get_type(
    const struct base_string *type_name,
    struct type_registry *tr
) {

    struct object *type = calloc(1, sizeof *type);
    type->kind = OBJECT_TYPE;
    type->parent = NULL; // TODO
    
    if(base_string_equals_raw(type_name, "type")){
        type->value_type = type;
    } else {
        struct base_string *type_string = base_string_new("type");
        type->value_type = type_registry_get_type(tr, type_string);
        base_string_destroy(type_string);
    }
        
    if(is_builtin_type(type_name)) {
        type->constant = true;
        type->attributes = NULL;
    } else {
        type->constant = false;
        type->attributes = base_map_new();
    }       
    
    return type;
}

static enum object_kind get_kind(const struct base_string *type_name) {
    if(base_string_equals_raw(type_name, "bool")){      return OBJECT_BOOL; }
    if(base_string_equals_raw(type_name, "double")){    return OBJECT_DOUBLE; }
    if(base_string_equals_raw(type_name, "function")){  return OBJECT_FUNCTION; }
    if(base_string_equals_raw(type_name, "int")){       return OBJECT_INTEGER; }
    if(base_string_equals_raw(type_name, "map")){       return OBJECT_MAP; }
    if(base_string_equals_raw(type_name, "null")){      return OBJECT_NULL ; }
    if(base_string_equals_raw(type_name, "string")){    return OBJECT_STRING; }
    if(base_string_equals_raw(type_name, "type")){      return OBJECT_TYPE; }
    if(base_string_equals_raw(type_name, "vector")){    return OBJECT_VECTOR; }
    return OBJECT_STRUCTURE;
}


struct object *object_new(
    const struct base_string *type_name,
    const struct base_string *instance_name,
    struct type_registry *tr
) {
    if(base_string_equals_raw(type_name, "type")){
        return get_type(instance_name, tr);        
    }

    struct object *o = calloc(1, sizeof *o);
    o->value_type = type_registry_get_type(tr, type_name);
    o->kind = get_kind(type_name);
    o->constant = false;
    o->attributes = base_map_new();
    o->parent = NULL; 
    return o;
}

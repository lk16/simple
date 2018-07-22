#include "object.h"

#include "base_map.h"
#include "type_registry.h"

#include <malloc.h>
#include <assert.h>

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
        struct structure *value_structure;
    };
    struct object *type;
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
        type->type = type;
    } else {
        struct base_string *type_string = base_string_new("type");
        type->type = type_registry_get_type(tr, type_string);
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
    o->type = type_registry_get_type(tr, type_name);
    o->kind = get_kind(type_name);
    o->attributes = base_map_new();
    o->parent = NULL; 
    
    if(base_string_equals_raw(type_name, "string")){
        o->constant = true;
    } else {
        o->constant = false;
    }
    return o;
}

struct object *object_copy(const struct object *o) {
    // TODO
    (void)o;
    return NULL;
}

struct object *object_get_attribute(
    struct object *o,
    const struct base_string *key
) {
    struct object *attribute;
    struct object *ancestor = o;
    
    while(ancestor) {
        if(ancestor->attributes && (attribute = base_map_get(ancestor->attributes, key))) {
            return attribute;
        }
        ancestor = ancestor->parent;
    }
  
    // TODO return error
    return NULL;
}

struct object * object_set_attribute(
    struct object *o,
    const struct base_string *key,
    struct object *value
) {
    // TODO key sanity check
    
    struct object *attribute;
    struct object *ancestor = o;
    
    while(ancestor) {
        if(ancestor->attributes && (attribute = base_map_get(ancestor->attributes, key))) {
            if(ancestor->constant){
                // TODO return error
                return NULL;
            }
            base_map_set(ancestor->attributes, key, value);
            // TODO return null object
            return NULL;
        }
        ancestor = ancestor->parent;
    }

    if(o->constant) {
        // TODO return error
        return NULL;
    }
 
    base_map_set(o->attributes, key,value);
    // TODO return null object
    return NULL;
}

struct object *object_get_type(
    struct object *o,
    const struct object *args
) {
    assert(args->kind == OBJECT_NULL);
    return o->type;    
}

struct object *object_has_type(
    struct object *o,
    const struct object *args
) {
    assert(args->kind == OBJECT_TYPE);
    struct object *ancestor = o;
    struct object *parent;
    
    while(ancestor) {
         if(ancestor->type == args) {
             // TODO return true object
             return NULL;
         }
         parent = object_get_parent(ancestor, NULL);
         if(ancestor == parent) {
             break;
         }         
    }
    
    // TODO return false object
    return NULL;
}

struct object *object_get_parent(
    struct object *o,
    const struct object *args
) {
    (void)args;
    return o->parent;
}

struct object *object_cast(
    struct object *o,
    const struct object *args
) {
    assert(args->kind == OBJECT_TYPE);
    struct object *ancestor = o;
    struct object *parent;
    
    while(ancestor) {
         if(ancestor->type == args) {
             return ancestor;
         }
         parent = object_get_parent(ancestor, NULL);
         if(ancestor == parent) {
             break;
         }         
    }
    
    // TODO cast to string or bool
    
    // TODO return error
    return NULL;
}

struct object *object_assign(
    struct object *o,
    const struct object *args
) {
    if(o->type != args->type) {
        // TODO return error
        return NULL;
    }

    // copy from args to o
    // TODO return null object
    return NULL;
}

struct object *object_hash(const struct object *o);
struct object *object_equals(struct object *o, const struct object *args);
struct object *object_not_equals(struct object *o, const struct object *args);

struct object *object_less_than(struct object *o, const struct object *args);
struct object *object_equals(struct object *o, const struct object *args);
struct object *object_greater_than(struct object *o, const struct object *args);
struct object *object_greater_or_equal(struct object *o, const struct object *args);
struct object *object_less_or_equal(struct object *o, const struct object *args);

struct object *object_add(struct object *o, const struct object *args);
struct object *object_add_assign(struct object *o, const struct object *args);

struct object *object_subtract(struct object *o, const struct object *args);
struct object *object_subtract_assign(struct object *o, const struct object *args);

struct object *object_multiply(struct object *o, const struct object *args);
struct object *object_multiply_assign(struct object *o, const struct object *args);

struct object *object_divide(struct object *o, const struct object *args);
struct object *object_divide_assign(struct object *o, const struct object *args);





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
        double value_double;
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
    const struct base_string *type_name
) {

    struct object *type = calloc(1, sizeof *type);
    type->kind = OBJECT_TYPE;
    type->parent = NULL; // TODO
    
    if(base_string_equals_raw(type_name, "type")){
        type->type = type;
    } else {
        struct base_string *type_string = base_string_new("type");
        type->type = type_registry_get_type(type_string);
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
    const struct base_string *instance_name
) {
    if(base_string_equals_raw(type_name, "type")){
        return get_type(instance_name);        
    }

    struct object *o = calloc(1, sizeof *o);
    o->type = type_registry_get_type(type_name);
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

static struct object *object_new_int(int value) {
    struct base_string *int_string = base_string_new("int");
    struct object *o = object_new(int_string, NULL);
    base_string_destroy(int_string);
    o->value_integer = value;
    return o;
}

static struct object *object_new_bool(bool value) {
    struct base_string *bool_string = base_string_new("bool");
    struct object *o = object_new(bool_string, NULL);
    base_string_destroy(bool_string);
    o->value_boolean = value;
    return o;
}

static struct object *object_new_null(void) {
    struct base_string *null_string = base_string_new("null");
    struct object *o = object_new(null_string, NULL);
    base_string_destroy(null_string);
    return o;
}

static struct object *object_new_double(double value) {
    struct base_string *double_string = base_string_new("double");
    struct object *o = object_new(double_string, NULL);
    base_string_destroy(double_string);
    o->value_double = value;
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
            return object_new_null();
        }
        ancestor = ancestor->parent;
    }

    if(o->constant) {
        // TODO return error
        return NULL;
    }
 
    base_map_set(o->attributes, key,value);
    return object_new_null();
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
             return object_new_bool(true);
         }
         parent = object_get_parent(ancestor, NULL);
         if(ancestor == parent) {
             break;
         }         
    }
    
    return object_new_bool(false);
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
    
    // TODO integer/bool/float conversion
    
    // TODO anyting to string conversion
    
    // TODO return error
    return NULL;
}

struct object *object_assign(
    struct object *o,
    const struct object *args
) {
    switch(o->kind){
        case OBJECT_NULL:
            break;
        case OBJECT_BOOL:
            o->value_boolean = args->value_boolean;
            break;
        case OBJECT_INTEGER:
            o->value_integer = args->value_integer;
            break;
        case OBJECT_DOUBLE:
            o->value_double = args->value_double;
            break;
        case OBJECT_STRING:
            // TODO implement string copy
            break;
        case OBJECT_VECTOR:
            // TODO implement vector
            break;
        case OBJECT_MAP:
            // TODO implement map
            break;
        case OBJECT_FUNCTION:
            o->value_function = args->value_function;
            break;
        case OBJECT_TYPE:
            o->type = args->type;
            break;
        case OBJECT_STRUCTURE:
            // TODO return error
            break;
    }
    
    return object_new_null();
}

struct object *object_hash(
    struct object *o,
    const struct object *args
) {
    assert(args->kind == OBJECT_NULL);
    
    if(!o->constant) {
        // TODO return error
        return NULL;
    }

    switch(o->kind){
        case OBJECT_NULL:
            return object_new_int(-1);
        case OBJECT_BOOL:
            return object_new_int(o->value_boolean ? 1 : 0);
        case OBJECT_INTEGER:
            return object_new_int(o->value_integer ^ 0x3e5313a0);
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_DOUBLE:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
}

struct object *object_equals(
    struct object *o,
    const struct object *args
) {
    if(args->kind != o->kind) {
        // TODO return error
        return NULL;
    }
    
    switch(o->kind){
        case OBJECT_NULL:
            return object_new_bool(true);
        case OBJECT_BOOL:
            return object_new_bool(o->value_boolean == args->value_boolean);
        case OBJECT_INTEGER:
            return object_new_bool(o->value_integer == args->value_integer);
        case OBJECT_STRING:
            return object_new_bool(base_string_equals(o->value_string, args->value_string));
        case OBJECT_FUNCTION:
            return object_new_bool(o->value_function == args->value_function);
        case OBJECT_MAP:
            // TODO map
            return NULL;
        case OBJECT_VECTOR:
            // TODO vector
            return NULL;
        case OBJECT_TYPE:
            return object_new_bool(o->type == args->type);
        case OBJECT_DOUBLE:
        case OBJECT_STRUCTURE:
            // TODO return error
            return NULL;
    }
}

struct object *object_not_equals(
    struct object *o,
    const struct object *args
) {
    struct object *result = object_equals(o, args);
    
    if(result->kind != OBJECT_BOOL) {
        return result;
    }
    
    result->value_boolean = !result->value_boolean;
    return result;
}

static struct object *object_less_than_const(
    const struct object *o,
    const struct object *args
) {

    if(args->kind != o->kind) {
        // TODO return error
        return NULL;
    }
    
    switch(o->kind){
        case OBJECT_NULL:
            return object_new_bool(false);
        case OBJECT_BOOL:
            return object_new_bool(o->value_boolean < args->value_boolean);
        case OBJECT_INTEGER:
            return object_new_bool(o->value_integer < args->value_integer);
        case OBJECT_DOUBLE:
            return object_new_bool(o->value_double < args->value_double);
        case OBJECT_STRING:
            return object_new_bool(strcmp(base_string_raw(o->value_string), base_string_raw(args->value_string)) < 0);
        case OBJECT_TYPE:
            return object_new_bool(o->type == args->type);
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_STRUCTURE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
}
    
struct object *object_less_than(
    struct object *o,
    const struct object *args
) {
    return object_less_than_const(o, args);
}

struct object *object_greater_than(
    struct object *o,
    const struct object *args
) {
    return object_less_than_const(args, o);
}

struct object *object_greater_or_equal(
    struct object *o,
    const struct object *args
) {
    struct object *result = object_less_than_const(o, args);
    
    if(result->kind != OBJECT_BOOL) {
        return result;
    }
    
    result->value_boolean = !result->value_boolean;
    return result;
}

struct object *object_less_or_equal(
    struct object *o,
    const struct object *args
) {
    struct object *result = object_less_than_const(args, o);
    
    if(result->kind != OBJECT_BOOL) {
        return result;
    }
    
    result->value_boolean = !result->value_boolean;
    return result; 
}

struct object *object_add(
    struct object *o,
    const struct object *args
) {
    if(args->kind != o->kind) {
        // TODO return error
        return NULL;
    }
    
    switch(o->kind){
        case OBJECT_INTEGER:
            return object_new_int(o->value_integer + args->value_integer);
        case OBJECT_DOUBLE:
            return object_new_double(o->value_double + args->value_double);
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
}
struct object *object_add_assign(struct object *o, const struct object *args) {
    struct object *result = object_add(o, args);
    
    // TODO check if an error occurred
    
        switch(o->kind){
        case OBJECT_INTEGER:
            o->value_integer = result->value_integer;
            break;
        case OBJECT_DOUBLE:
            o->value_double = result->value_double;
            break;
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
    
    return result;
}

struct object *object_subtract(
    struct object *o,
    const struct object *args
) {

    if(args->kind != o->kind) {
        // TODO return error
        return NULL;
    }
    
    switch(o->kind){
        case OBJECT_INTEGER:
            return object_new_int(o->value_integer - args->value_integer);
        case OBJECT_DOUBLE:
            return object_new_double(o->value_double - args->value_double);
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
}

struct object *object_subtract_assign(
    struct object *o,
    const struct object *args
) {
    struct object *result = object_subtract(o, args);
    
    // TODO check if an error occurred
    
    switch(o->kind){
        case OBJECT_INTEGER:
            o->value_integer = result->value_integer;
            break;
        case OBJECT_DOUBLE:
            o->value_double = result->value_double;
            break;
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
    
    return result;
}

struct object *object_multiply(
    struct object *o,
    const struct object *args
) {
    
    if(args->kind != o->kind) {
        // TODO return error
        return NULL;
    }
    
    switch(o->kind){
        case OBJECT_INTEGER:
            return object_new_int(o->value_integer * args->value_integer);
        case OBJECT_DOUBLE:
            return object_new_double(o->value_double * args->value_double);
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
}

struct object *object_multiply_assign(
    struct object *o,
    const struct object *args
) {
    
    struct object *result = object_multiply(o, args);
    
    // TODO check if an error occurred
    
    switch(o->kind){
        case OBJECT_INTEGER:
            o->value_integer = result->value_integer;
            break;
        case OBJECT_DOUBLE:
            o->value_double = result->value_double;
            break;
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
    
    return result;
}

struct object *object_divide(
    struct object *o,
    const struct object *args
) {
    if(args->kind != o->kind) {
        // TODO return error
        return NULL;
    }

    switch(o->kind){
        case OBJECT_INTEGER:
            if(args->value_integer == 0) {
                // TODO return error
                return NULL;
            }
            return object_new_int(o->value_integer / args->value_integer);
        case OBJECT_DOUBLE:
            return object_new_double(o->value_double / args->value_double);
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
}
struct object *object_divide_assign(
    struct object *o,
    const struct object *args
) {
    
    struct object *result = object_divide(o, args);
    
    // TODO check if an error occurred
    
    switch(o->kind){
        case OBJECT_INTEGER:
            o->value_integer = result->value_integer;
            break;
        case OBJECT_DOUBLE:
            o->value_double = result->value_double;
            break;
        case OBJECT_STRING:
            // TODO implement string
            return NULL;
        case OBJECT_BOOL:
        case OBJECT_FUNCTION:
        case OBJECT_MAP:
        case OBJECT_NULL:
        case OBJECT_STRUCTURE:
        case OBJECT_TYPE:
        case OBJECT_VECTOR:
            // TODO return error
            return NULL;
    }
    
    return result;
    
}

struct object *object_to_string(
    struct object *o,
    const struct object *args
) {
    
    (void)args;
    (void)o;
    
    // TODO implement with open_memstream()
    
    // TODO return error
    return NULL;
}


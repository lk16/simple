#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "type.h"

struct simple_error;

struct simple_error *simple_error_new(const char *filename, size_t line, const char *function, const char *format, ...);
void simple_error_set_child(struct simple_error *error, struct simple_error *child);
void simple_error_show(const struct simple_error *error, FILE *file);
void simple_error_destroy(struct simple_error *error);

struct simple_string;

struct simple_string *simple_string_new(const char *cstring);
void  simple_string_destroy(struct simple_string *string);
size_t simple_string_hash(const struct simple_string *string);
bool simple_string_startswith(const struct simple_string *string, const char *search);
bool simple_string_equals(const struct simple_string *lhs, const struct simple_string *rhs);
const char *simple_string_get(const struct simple_string *string);

struct simple_hashtable;

struct simple_hashtable *simple_hashtable_new(const struct type *key_type, const struct type *value_type);
void simple_hashtable_destroy(struct simple_hashtable *table);
struct simple_error *simple_hashtable_insert(struct simple_hashtable *table, const struct object *key, const struct object *value);
struct simple_error *simple_hashtable_find(struct simple_hashtable *table, const struct object *key, struct object **result);
struct simple_error *simple_hashtable_find_const(const struct simple_hashtable *table, const struct object *key, const struct object **result);
struct simple_error * simple_hashtable_upsert(struct simple_hashtable *table, const struct object *key, const struct object *value, bool *inserted);
struct simple_error *simple_hashtable_erase(struct simple_hashtable *table, const struct object *key, bool *erased);
size_t simple_hashtable_size(const struct simple_hashtable *table);

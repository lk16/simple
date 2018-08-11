#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "type.h"

struct simple_error;

struct simple_error *simple_error_new(const char *message, struct simple_error *child);
void simple_error_destroy(struct simple_error *error);

struct simple_iterator;

bool simple_iterator_next(struct simple_iterator *iterator, struct object **object);
void simple_iterator_destroy(struct simple_iterator *iterator);

struct simple_const_iterator;

bool simple_const_iterator_next(struct simple_const_iterator *iterator, const struct object **object);
void simple_const_iterator_destroy(struct simple_const_iterator *iterator);

struct simple_string;

struct simple_string *simple_string_new(const char *cstring);
struct simple_string *simple_string_new_combine(const struct simple_string *lhs, const struct simple_string *rhs);
struct simple_string *simple_string_substring(const struct simple_string, size_t start, size_t length);
void  simple_string_destroy(struct simple_string *string);
size_t simple_string_length(const struct simple_string *string);
char simple_string_at(const struct simple_string *string);
size_t simple_string_find(const struct simple_string *string, const struct simple_string *search);
bool simple_string_equals(const struct simple_string *lhs, const struct simple_string *rhs);
bool simple_string_less(const struct simple_string *lhs, const struct simple_string *rhs);
const char *simple_string_get(const struct simple_string *string);
struct simple_const_iterator *simple_string_get_const_iterator(const struct simple_string *string);

struct simple_vector;

struct simple_vector *simple_vector_new(const struct type *element_type);
struct simple_vector *simple_vector_copy(const struct simple_vector *original);
void simple_vector_destroy(struct simple_vector *vector);
size_t simple_vector_length(const struct simple_vector *vector);
struct simple_error *simple_vector_front(struct simple_vector *vector, struct object **result);
struct simple_error *simple_vector_front_const(const struct simple_vector *vector, struct object **result);
struct simple_error *simple_vector_back(struct simple_vector *vector, struct object **result);
struct simple_error *simple_vector_back_const(const struct simple_vector *vector, struct object **result);
struct simple_error *simple_vector_at(struct simple_vector *vector, size_t index, struct object **result);
struct simple_error *simple_vector_at_const(const struct simple_vector *vector, size_t index, struct object **result);
struct simple_error *simple_vector_pop(struct simple_vector *vector);
struct simple_error *simple_vector_push(struct simple_vector *vector, struct object *object);
struct simple_error *simple_vector_erase(struct simple_vector *vector, size_t index);
void simple_vector_clear(struct simple_vector *vector);
bool simple_vector_equals(const struct simple_vector *lhs, const struct simple_vector *rhs);
bool simple_vector_less(const struct simple_vector *lhs, const struct simple_vector *rhs);
struct simple_iterator *simple_vector_get_iterator(struct simple_string *string);
struct simple_const_iterator *simple_vector_get_const_iterator(const struct simple_string *string);

struct simple_hashtable;

struct simple_hashtable *simple_hashtable_new(const struct type *key_type, const struct type *value_type);
void simple_hashtable_destroy(struct simple_hashtable *table);
struct simple_error *simple_hashtable_insert(struct simple_hashtable *table, const struct object *key, const struct object *value);
struct simple_error *simple_hashtable_find(struct simple_hashtable *table, const struct object *key, struct object **result);
struct simple_error *simple_hashtable_find_const(const struct simple_hashtable *table, const struct object *key, struct object **result);
struct simple_error * simple_hashtable_upsert(struct simple_hashtable *table, const struct object *key, const struct object *value, bool *inserted);
struct simple_error *simple_hashtable_erase(struct simple_hashtable *table, const struct object *key, bool *erased);
size_t simple_hashtable_size(const struct simple_hashtable *table);
struct simple_iterator *simple_hashtable_entries_iterator(struct simple_hashtable *table);
struct simple_const_iterator *simple_hashtable_entries_const_iterator(const struct simple_hashtable *table);

struct simple_set;

struct simple_set *simple_set_new(const struct type *type);
void simple_set_destroy(struct simple_set *set);
struct simple_error *simple_set_find_const(const struct simple_set *table, const struct object *key, struct object **result);
struct simple_error *simple_set_insert(struct simple_set *set, const struct object *object, bool *inserted);
struct simple_error *simple_set_erase(struct simple_set *table, const struct object *key, bool *erased);
size_t simple_set_size(const struct simple_set *table);
struct simple_iterator *simple_hashtable_get_iterator(struct simple_set *set);
struct simple_const_iterator *simple_hashtable_get_const_iterator(const struct simple_set *set);

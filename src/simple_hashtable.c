#include <malloc.h>
#include <string.h>

#include "simple_hashtable.h"
#include "simple_object.h"

struct simple_hashtable_entry {
    struct object *key, *value;
    struct simple_hashtable_entry *next;
};

static void simple_hashtable_entry_destroy(
    struct simple_hashtable_entry *entry
) {
    object_refcount_decrease(entry->key);
    object_refcount_decrease(entry->value);
    free(entry);
}

static struct simple_error *simple_hashtable_entry_new(
    const struct object *key,
    const struct object *value,
    struct simple_hashtable_entry *next,
    struct simple_hashtable_entry **result
) {
    struct simple_hashtable_entry *entry = calloc(1, sizeof *entry);
    entry->next = next;

    struct simple_error *error;
    error = object_copy(key, &entry->key);
    simple_error_check(error);

    error = object_copy(value, &entry->value);
    simple_error_check(error);

    *result = entry;
    return NULL;

    cleanup:
    simple_hashtable_entry_destroy(entry);
    return error;
}


struct simple_hashtable {
    const struct type *key_type, *value_type;
    struct simple_hashtable_entry **buckets;
    size_t bucket_count;
    size_t size;
};

struct simple_hashtable *simple_hashtable_new(
    const struct type *key_type,
    const struct type *value_type
) {
    struct simple_hashtable *table = calloc(1, sizeof *table);
    *table = (struct simple_hashtable) {
        .bucket_count = 15,
        .size = 0,
        .key_type = key_type,
        .value_type = value_type
    };
    table->buckets = calloc(table->bucket_count, sizeof *table->buckets);
    return table;
}

void simple_hashtable_destroy(struct simple_hashtable *table) {
    for (size_t bucket_id=0; bucket_id < table->bucket_count; bucket_id++) {
        struct simple_hashtable_entry *entry, *next;
        entry = table->buckets[bucket_id];
        while (entry) {
            next = entry->next;
            simple_hashtable_entry_destroy(entry);
            entry = next;
        }
    }
    free(table->buckets);
    free(table);
}

static double simple_hashtable_load_factor(
    const struct simple_hashtable *table
) {
    return ((double)table->size) / table->bucket_count;
}

static struct simple_error *simple_hashtable_get_bucket(
    const struct simple_hashtable *table,
    const struct object *key,
    size_t *result
) {
    struct simple_error *error = NULL;
    error = object_check_type(key, table->key_type);
    simple_error_check(error);

    *result = object_get_hash(key) % table->bucket_count;

    cleanup:
    if (error) {
        *result = 0;
    }
    return error;

    return NULL;
} __attribute__((warn_unused_result))

static struct simple_error *simple_hashtable_rehash(
    struct simple_hashtable *table
) {
    struct simple_hashtable_entry **old_buckets = table->buckets;
    size_t old_bucket_count = table->bucket_count;
    table->bucket_count = (table->bucket_count * 2) - 1;
    table->buckets = calloc(table->bucket_count, sizeof *table->buckets);

    struct simple_error *error;

    for (size_t i=0; i<old_bucket_count; i++) {
        struct simple_hashtable_entry *entry, *next;
        entry = old_buckets[i];
        while (entry) {
            error = simple_hashtable_insert(table, entry->key, entry->value);
            simple_error_check(error);
            next = entry->next;
            simple_hashtable_entry_destroy(entry);
            entry = next;
        }
    }
    free(old_buckets);
    return NULL;

    cleanup:
    // TODO this leaks a lot, redo this
    return error;



} __attribute__((warn_unused_result))

struct simple_error *simple_hashtable_find(
    struct simple_hashtable *table,
    const struct object *key,
    struct object **result
) {

    size_t bucket_id;
    struct simple_error *error;

    error = simple_hashtable_get_bucket(table, key, &bucket_id);
    simple_error_check(error);

    const struct simple_hashtable_entry *entry = table->buckets[bucket_id];

    while (entry) {
        bool equals;
        object_equals(entry->key, key, &equals);
        if (equals) {
            object_refcount_increase(entry->value);
            *result = entry->value;
            return NULL;
        }
        entry = entry->next;
    }

    cleanup:
    *result = NULL;
    return error;
}

struct simple_error *simple_hashtable_insert(
    struct simple_hashtable *table,
    const struct object *key,
    const struct object *value
) {
    size_t bucket_id;
    struct simple_error *error;

    error = simple_hashtable_get_bucket(table, key, &bucket_id);
    simple_error_check(error);

    struct simple_hashtable_entry *entry = table->buckets[bucket_id];

    while (entry) {
        bool equals;
        error = object_equals(entry->key, key, &equals);
        simple_error_check(error);
        if (equals) {
            break;
        }
        entry = entry->next;
    }

    if (entry) {
        object_refcount_decrease(entry->value);
        error = object_copy(value, &entry->value);
    } else {
        struct simple_hashtable_entry **bucket_head;
        bucket_head = table->buckets + bucket_id;
        error = simple_hashtable_entry_new(key, value, *bucket_head,
            bucket_head);
        simple_error_check(error);
        table->size++;
    }

    if (simple_hashtable_load_factor(table) > 0.75) {
        error = simple_hashtable_rehash(table);
        simple_error_check(error);
    }

    cleanup:
    return error;
}

struct simple_error *simple_hashtable_erase(
    struct simple_hashtable *table,
    const struct object *key,
    bool *erased
) {
    size_t bucket_id;
    struct simple_error *error;
    error = simple_hashtable_get_bucket(table, key, &bucket_id);
    simple_error_check(error);

    struct simple_hashtable_entry **entry = table->buckets + bucket_id;

    while (*entry) {
        bool equals;
        object_equals((*entry)->key, key, &equals);
        simple_hashtable_entry_destroy(*entry);
        if (equals) {
            *entry = (*entry)->next;
            *erased = true;
            table->size--;
            return NULL;
        }
        entry = &((*entry)->next);
    }
    *erased = false;

    cleanup:
    return error;
}

size_t simple_hashtable_size(
    const struct simple_hashtable *table
) {
    return table->size;
}

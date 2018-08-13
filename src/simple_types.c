#include <malloc.h>
#include <string.h>

#include "simple_types.h"

struct simple_error {
    struct simple_error *child;
    const char *filename;
    size_t line;
    const char *function;
    char *message;
};

struct simple_error *simple_error_new(
    const char *filename,
    size_t line,
    const char *function,
    const char *format,
    ...
) {
    struct simple_error *error = calloc(1, sizeof *error);

    *error = (struct simple_error) {
        .filename = filename,
        .line = line,
        .function = function,
        .child = NULL,
        .message = NULL
    };

    va_list args, args2;
    va_start(args, format);
    size_t message_length = (size_t)vsnprintf(error->message, 0, format, args);
    va_end (args);

    error->message = calloc(message_length + 1, sizeof *error->message);

    va_start(args2, format);
    vsnprintf(error->message, message_length, format, args2);
    va_end (args2);

    return error;
}

void simple_error_set_child(
    struct simple_error *error,
    struct simple_error *child
) {
    error->child = child;
}

static void simple_error_show_line(
    const struct simple_error *error,
    FILE *file
) {
    fprintf(file, "%s:%zu %s() %s\n", error->filename, error->line,
    error->function, error->message);
}

void simple_error_show(
    const struct simple_error *error,
    FILE *file
) {
    if(!error) {
        return;
    }
    fprintf(file, "%s\n", "An error occurred, stacktrace:");
    while(error) {
        simple_error_show_line(error, file);
        error = error->child;
    }
}

void simple_error_destroy(
    struct simple_error *error
) {
    if(error->child) {
        simple_error_destroy(error->child);
    }
    free(error->message);
    free(error);
}

struct simple_string {
    char *cstring;
    size_t length;
};

struct simple_string *simple_string_new(
    const char *cstring
) {
    struct simple_string *string = calloc(1, sizeof *string);
    string->length = strlen(cstring);
    string->cstring = calloc(string->length + 1, sizeof *string->cstring);
    memcpy(string->cstring, cstring, string->length + 1);
    return string;
}

struct simple_string *simple_string_copy(
    const struct simple_string *string
) {
    return simple_string_new(simple_string_get(string));
}

void simple_string_destroy(
    struct simple_string *string
) {
    free(string->cstring);
    free(string);
}

const char *simple_string_get(const struct simple_string *string) {
    return string->cstring;
}

bool simple_string_equals(
    const struct simple_string *lhs,
    const struct simple_string *rhs
) {
    if(lhs->length != rhs->length) {
        return false;
    }
    return strcmp(lhs->cstring, rhs->cstring) == 0;
}

bool simple_string_startswith(
    const struct simple_string *string,
    const char *search
) {
    return strncmp(string->cstring, search, strlen(search)) == 0;
}

size_t simple_string_hash(
    const struct simple_string *string
) {
    size_t hash = 8937;
    for(size_t i=0; i<string->length; i++) {
        hash += (size_t)string->cstring[i];
        hash *= (size_t)123457;
    }
    return hash;
}

struct simple_hashtable_entry {
    struct object *key, *value;
    struct simple_hashtable_entry *next;
};

static struct simple_hashtable_entry *simple_hashtable_entry_new(
    const struct object *key,
    const struct object *value,
    struct simple_hashtable_entry *next
) {
    struct simple_hashtable_entry *entry = calloc(1, sizeof *entry);
    *entry = (struct simple_hashtable_entry) {
        .key = object_copy(key),
        .value = object_copy(value),
        .next = next
    };
    return entry;
}

static void simple_hashtable_entry_destroy(
    struct simple_hashtable_entry *entry
) {
    object_destroy(entry->key);
    object_destroy(entry->value);
    free(entry);
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
    for(size_t bucket_id=0; bucket_id < table->bucket_count; bucket_id++) {
        struct simple_hashtable_entry *entry, *next;
        entry = table->buckets[bucket_id];
        while(entry) {
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
    const struct object *key, size_t *result
) {
    if(!object_has_type(key, table->key_type)) {
        *result = 0;
        return simple_error_new(__FILE__, __LINE__, __FUNCTION__, "%s",
            "Invalid key type provided.");
    }
    *result = object_get_hash(key) % table->bucket_count;
    return NULL;
}

static void simple_hashtable_rehash(
    struct simple_hashtable *table
) {
    struct simple_hashtable_entry **old_buckets = table->buckets;
    size_t old_bucket_count = table->bucket_count;
    table->bucket_count = (table->bucket_count * 2) - 1;
    table->buckets = calloc(table->bucket_count, sizeof *table->buckets);

    for(size_t i=0; i<old_bucket_count; i++) {
        struct simple_hashtable_entry *entry, *next;
        entry = old_buckets[i];
        while(entry) {
            simple_hashtable_insert(table, entry->key, entry->value);
            next = entry->next;
            simple_hashtable_entry_destroy(entry);
            entry = next;
        }
    }
    free(old_buckets);
}

struct simple_error *simple_hashtable_find(
    struct simple_hashtable *table,
    const struct object *key,
    struct object **result
) {

    size_t bucket_id;
    struct simple_error *child_error = simple_hashtable_get_bucket(table, key,
        &bucket_id);

    if(child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s",
            "Could not determine if key is present in hashtable.");
        simple_error_set_child(error, child_error);
        return error;
    }

    const struct simple_hashtable_entry *entry = table->buckets[bucket_id];

    while(entry) {
        if(object_equals(entry->key, key)) {
            *result = entry->value;
            return NULL;
        }
        entry = entry->next;
    }

    *result = NULL;
    return NULL;
}

struct simple_error *simple_hashtable_find_const(
    const struct simple_hashtable *table,
    const struct object *key,
    const struct object **result
) {
    size_t bucket_id;
    struct simple_error *child_error = simple_hashtable_get_bucket(table, key,
        &bucket_id);

    if(child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s",
            "Could not determine if key is present in hashtable.");
        simple_error_set_child(error, child_error);
        return error;
    }

    struct simple_hashtable_entry **entry_ptr = table->buckets + bucket_id;
    while(*entry_ptr) {
        if(object_equals((*entry_ptr)->key, key)) {
            *result = (*entry_ptr)->value;
            return NULL;
        }
        *entry_ptr = (*entry_ptr)->next;
    }
    return NULL;
}

struct simple_error *simple_hashtable_insert(
    struct simple_hashtable *table,
    const struct object *key,
    const struct object *value
) {
    size_t bucket_id;
    struct simple_error *child_error = simple_hashtable_get_bucket(table, key,
        &bucket_id);

    if(child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s",
            "Could not determine if key is present in hashtable.");
        simple_error_set_child(error, child_error);
        return error;
    }

    struct simple_hashtable_entry *entry = table->buckets[bucket_id];

    while(entry) {
        if(object_equals(entry->key, key)) {
            break;
        }
        entry = entry->next;
    }

    if(entry) {
        entry->value = object_copy(value);
    } else {
        struct simple_hashtable_entry **bucket_head;
        bucket_head = table->buckets + bucket_id;
        *bucket_head = simple_hashtable_entry_new(key, value, *bucket_head);
    }

    if(simple_hashtable_load_factor(table) > 0.75) {
        simple_hashtable_rehash(table);
    }
    return NULL;
}

struct simple_error *simple_hashtable_erase(
    struct simple_hashtable *table,
    const struct object *key,
    bool *erased
) {
    size_t bucket_id;
    struct simple_error *child_error = simple_hashtable_get_bucket(table, key,
        &bucket_id);

    if(child_error) {
        struct simple_error *error = simple_error_new(__FILE__, __LINE__,
            __FUNCTION__, "%s",
            "Could not determine if key is present in hashtable.");
        simple_error_set_child(error, child_error);
        return error;
    }

    struct simple_hashtable_entry **entry = table->buckets + bucket_id;

    while(*entry) {
        if(object_equals((*entry)->key, key)) {
            *entry = (*entry)->next;
            *erased = true;
            return NULL;
        }
        entry = &((*entry)->next);
    }
    *erased = false;
    return NULL;
}

size_t simple_hashtable_size(
    const struct simple_hashtable *table
) {
    return table->size;
}

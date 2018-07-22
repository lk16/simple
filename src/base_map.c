#include <malloc.h>

#include "base_map.h"

struct base_map_entry {
    struct base_string *key;
    struct object *value;
    struct base_map_entry *next;
};

static struct base_map_entry *base_map_entry_new(
    const struct base_string *key,
    struct object *value,
    struct base_map_entry *next
) {
    struct base_map_entry *bme = calloc(1, sizeof *bme);
    bme->key = base_string_copy(key);
    bme->value = value;
    bme->next = next;
    return bme;
}

static void base_map_entry_destroy(
    struct base_map_entry *bme
) {
    struct base_map_entry *next;

    while(bme) {
        next = bme->next;
        base_string_destroy(bme->key);
        free(bme);
        bme = next;
    }
}

struct base_map {
    struct base_map_entry **buckets;
    size_t bucket_count;
    size_t size;
};

struct base_map *base_map_new(void) {
    struct base_map *bm = calloc(1, sizeof *bm);
    bm->bucket_count = 15;
    bm->size = 0;
    bm->buckets = calloc(bm->bucket_count, sizeof *bm->buckets);
    return bm;
}

void base_map_destroy(struct base_map *bm) {
    for(size_t i=0; i<bm->bucket_count; i++) {
        base_map_entry_destroy(bm->buckets[i]);
    }
    free(bm->buckets);
    free(bm);
}

static double base_map_load_factor(const struct base_map *bm) {
    return ((double)bm->size) / bm->bucket_count;
}

static void base_map_rehash(struct base_map *bm) {
    struct base_map_entry **old_buckets = bm->buckets;
    size_t old_bucket_count = bm->bucket_count;
    bm->bucket_count = (bm->bucket_count * 2) - 1;
    bm->buckets = calloc(bm->bucket_count, sizeof *bm->buckets);

    for(size_t i=0; i<old_bucket_count; i++) {
        struct base_map_entry *entry = old_buckets[i];
        while(entry) {
            base_map_set(bm, entry->key, entry->value);
            entry = entry->next;
        }
        base_map_entry_destroy(old_buckets[i]);
    }
    free(old_buckets);
}

static struct base_map_entry *base_map_find(
    struct base_map *bm,
    const struct base_string *key
) {
    size_t bucket_id = base_string_hash(key) % bm->bucket_count;
    struct base_map_entry *entry = bm->buckets[bucket_id];

    while(entry) {
        if(base_string_equals(entry->key, key)) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

void base_map_set(
    struct base_map *bm,
    const struct base_string *key,
    struct object *value
) {

    struct base_map_entry *found = base_map_find(bm,key);
    if(found) {
        found->value = value;
        return;
    }

    size_t bucket_id = base_string_hash(key) % bm->bucket_count;
    struct base_map_entry **bucket_head = bm->buckets + bucket_id;
    *bucket_head = base_map_entry_new(key, value, *bucket_head);
    bm->size++;

    if(base_map_load_factor(bm) > 0.75) {
        base_map_rehash(bm);
    }
}

struct object *base_map_get(
    struct base_map *bm,
    const struct base_string *key
) {
    struct base_map_entry *found = base_map_find(bm,key);

    if(found) {
        return found->value;
    }
    return NULL;
}

void base_map_remove(
    struct base_map* bm,
    const struct base_string* key
) {
    size_t bucket_id = base_string_hash(key) % bm->bucket_count;
    struct base_map_entry **entry = &bm->buckets[bucket_id];
    
    while(*entry) {
        if(base_string_equals((*entry)->key, key)) {
            struct base_map_entry *removed_entry = *entry;
            *entry = (*entry)->next;
            removed_entry->next = NULL;
            base_map_entry_destroy(removed_entry);
        }
        entry = &(*entry)->next;
    }
}

size_t base_map_size(const struct base_map* bm)
{
    return bm->size;
}

struct base_map_iterator {
    struct base_map *bm;
    size_t bucket_id;
    struct base_map_entry *entry;
    const struct base_string **output_key;
    struct object **output_value;
};

struct base_map_iterator *base_map_iterator_new(
    struct base_map *bm,
    const struct base_string **key,
    struct object **value
) {
    struct base_map_iterator *bmi = calloc(1, sizeof *bmi);
    bmi->bm = bm;
    bmi->bucket_id = 0;
    bmi->entry = bm->buckets[0];
    bmi->output_key = key;
    bmi->output_value = value;
    return bmi;
}

bool base_map_iterator_next(
    struct base_map_iterator *bmi
) {
    struct base_map_entry *entry = bmi->entry;

    if(entry) {
        bmi->entry = entry->next;
        *bmi->output_key = entry->key;
        *bmi->output_value = entry->value;
        return true;
    }

    size_t bucket_id = bmi->bucket_id + 1;
    size_t bucket_count = bmi->bm->bucket_count;

    while(bucket_id < bucket_count) {
        entry = bmi->bm->buckets[bucket_id];

        if(entry) {
            bmi->bucket_id = bucket_id;
            bmi->entry = entry->next;
            *bmi->output_key = entry->key;
            *bmi->output_value = entry->value;
            return true;
        }

        bucket_id++;
    }

    return false;
}

void base_map_iterator_destroy(
    struct base_map_iterator *bmi
) {
    free(bmi);
}

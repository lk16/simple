#include "base_string.h"

#include <malloc.h>

struct base_string {
    size_t length;
    char *raw;
    size_t hash;
};

struct base_string *base_string_new(const char *raw) {
    struct base_string *bs = malloc(sizeof *bs);
    bs->length = strlen(raw);
    bs->raw = malloc(bs->length + 1);
    strcpy(bs->raw, raw);
    bs->hash = 0;
    for(size_t i=0; i<bs->length; i++){
        bs->hash += (size_t)(bs->raw[i]);
        bs->hash *= 123457;
    }
    return bs;
}

void base_string_destroy(struct base_string *bs) {
    free(bs->raw);
    free(bs);
}

size_t base_string_hash(const struct base_string *bs) {
    return bs->hash;
}

bool base_string_equals(
    const struct base_string *lhs,
    const struct base_string *rhs
) {
    if(lhs->hash != rhs->hash) {
        return false;
    }

    if(lhs->length != rhs->length) {
        return false;
    }

    return strcmp(base_string_raw(lhs), base_string_raw(rhs)) == 0;
}

const char *base_string_raw(const struct base_string *bs) {
    return bs->raw;
}

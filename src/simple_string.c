#include "simple_string.h"

#include "type.h"
#include "simple_error.h"

#include <malloc.h>
#include <string.h>


struct simple_string {
    char *cstring;
    size_t length;
};

struct simple_string *simple_string_new(
    const char *cstring
) {
    struct simple_string *string = calloc(1, sizeof *string);
    string->length = strlen(cstring);
    string->cstring = calloc(string->length+ 1, sizeof *string->cstring);
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
    if (lhs->length != rhs->length) {
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
    for (size_t i=0; i<string->length; i++) {
        hash += (size_t)string->cstring[i];
        hash *= (size_t)123457;
    }
    return hash;
}

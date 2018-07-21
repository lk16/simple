#include <stdio.h>

#include "base_string.h"

int main(int argc, char **argv) {

    struct base_string *str, *str2;

    str = base_string_new("hello world!");
    str2 = base_string_new("hello world!");

    printf("hash: %ld %ld\n",
            base_string_hash(str),
            base_string_hash(str2)
    );

    printf("raw: %s\n", base_string_raw(str));
    printf("raw: %s\n", base_string_raw(str2));

    printf("equal: %d\n", base_string_equals(str, str2) ? 1 : 0);

    base_string_destroy(str);
    base_string_destroy(str2);

    return 0;
}

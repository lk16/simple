#include "../simple_test.h"
#include "../simple_error.h"
#include "../simple_hashtable.h"
#include "../type.h"

static struct simple_error *test_hashtable_init(
    void
) {
    struct simple_error *error;
    const struct type *string_type;
    error = type_registry_get_type("string", &string_type);
    simple_error_forward(error, "%s", "");

    struct simple_hashtable *table;
    table = simple_hashtable_new(string_type, string_type);
    simple_hashtable_destroy(table);

    return NULL;
}



int main() {

    simple_test_init();

    struct simple_error *error;
    error = type_registry_new();
    if (error) {
        simple_error_show(error, stderr);
        simple_error_destroy(error);
    }

    struct simple_test_item *root, *hashtable;

    root = simple_test_get_root();
    hashtable = simple_test_create_node(root, "hashtable");
    simple_test_create_leaf(hashtable, "init", test_hashtable_init);

    simple_test_run();
    simple_test_destroy();

    error = type_registry_destroy();
    if (error) {
        simple_error_show(error, stderr);
        simple_error_destroy(error);
    }

    return 0;
}

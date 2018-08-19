#include <string.h>
#include <malloc.h>

#include "simple_test.h"

struct simple_test_item *simple_test_root;



enum simple_test_item_kind{
    SIMPLE_TEST_INTERNAL,
    SIMPLE_TEST_LEAF
};

struct simple_test_item{
    enum simple_test_item_kind kind;
    struct simple_test_item *parent, *next;
    char name[32];
    union {
        simple_test_func func;
        struct {
            struct simple_test_item *first_child, *last_child;
        };
    };
};

static void simple_test_item_destroy(
    struct simple_test_item *item
) {
    if (!item) {
        return;
    }

    if (item->kind == SIMPLE_TEST_INTERNAL) {
        simple_test_item_destroy(item->first_child);
    }

    simple_test_item_destroy(item->next);
    free(item);
}

static size_t simple_test_item_count(
    const struct simple_test_item *item
) {
    if (!item) {
        return 0;
    }

    size_t count = 1 + simple_test_item_count(item->next);
    if (item->kind == SIMPLE_TEST_INTERNAL) {
        count += simple_test_item_count(item->first_child);
    }
    return count;
}

void simple_test_init(
    void
) {
    simple_test_root = simple_test_create_node(NULL, "<root>");
}


static size_t simple_test_get_full_name(
    const struct simple_test_item *item,
    char *buff,
    size_t buff_len
) {
    if (!item) {
        return 0;
    }

    size_t used = 0;
    if (item->parent) {
        used = simple_test_get_full_name(item->parent, buff, buff_len);
    }

    buff_len -= used;
    buff += used;

    if (buff_len < 2) {
        buff[buff_len - 1] = '\0';
        return used;
    }

    if (item->parent) {
        buff[0] = '.';
        buff++;
        buff_len--;
        used++;
    }

    strncpy(buff, item->name, buff_len);
    size_t name_len = strlen(item->name);

    if (name_len >= buff_len - 1) {
        used += buff_len;
        buff[buff_len - 1] = '\0';
    } else {
        used += name_len;
    }
    return used;
}

static void simple_test_run_recursively(
    struct simple_test_item *item
) {
    if (!item) {
        return;
    }

    char buff[1024];
    simple_test_get_full_name(item, buff, 1024);
    printf("\033[0;33mVisiting %s\033[0m\n", buff);

    switch (item->kind) {
        case SIMPLE_TEST_INTERNAL:
            simple_test_run_recursively(item->first_child);
            break;
        case SIMPLE_TEST_LEAF:
            item->func();
            break;
    }

    simple_test_run_recursively(item->next);
}


void simple_test_run(
    void
) {
    simple_test_run_recursively(simple_test_root);
}

void simple_test_destroy(
    void
) {
    simple_test_item_destroy(simple_test_root);
}


void simple_test_create_leaf(
    struct simple_test_item *parent,
    const char *name,
    simple_test_func func
) {
    if (!parent) {
        fprintf(stderr, "Cannot create leaf '%s' to null parent", name);
        return;
    }

    if (parent->kind != SIMPLE_TEST_INTERNAL) {
        fprintf(stderr, "Cannot assign node '%s' to leaf.\n", name);
        return;
    }

    struct simple_test_item *item = calloc(1, sizeof *item);
    *item = (struct simple_test_item) {
        .kind = SIMPLE_TEST_LEAF,
        .next = NULL,
        .func = func,
        .parent = parent
    };

    strncpy(item->name, name, 32);

    if (parent->first_child) {
        parent->last_child->next = item;
    } else {
        parent->first_child = item;
    }

    parent->last_child = item;

}

struct simple_test_item *simple_test_create_node(
    struct simple_test_item *parent,
    const char *name
) {
    if (parent && parent->kind != SIMPLE_TEST_INTERNAL) {
        fprintf(stderr, "Cannot assign node '%s' to leaf.\n", name);
        return NULL;
    }

    if (strlen(name) > 31) {
        fprintf(stderr, "Cannot assign too long name '%s' to leaf.\n", name);
        return NULL;
    }

    struct simple_test_item *item = calloc(1, sizeof *item);
    *item = (struct simple_test_item) {
        .kind = SIMPLE_TEST_INTERNAL,
        .next = NULL,
        .parent = parent
    };

    strncpy(item->name, name, 32);

    if (parent) {
        if (parent->first_child) {
            parent->last_child->next = item;
        } else {
            parent->first_child = item;
        }
        parent->last_child = item;
    }

    return item;
}


struct simple_error *some_test() {
    printf("%s\n", "some_test()");
    return NULL;
}

struct simple_error *some_other_test() {
    printf("%s\n", "some_other_test()");
    return NULL;
}


int main() {
    simple_test_init();

    struct simple_test_item *group, *subgroup;

    group = simple_test_create_node(simple_test_root, "group");
    simple_test_create_leaf(group, "test1", some_test);
    simple_test_create_leaf(group, "test2", some_other_test);

    subgroup = simple_test_create_node(group, "subgroup");
    simple_test_create_leaf(subgroup, "test3", some_other_test);
    simple_test_create_leaf(subgroup, "test4", some_test);

    simple_test_create_leaf(simple_test_root, "test5", some_test);

    simple_test_run();
    simple_test_destroy();

    return 0;
}

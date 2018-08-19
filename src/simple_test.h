#pragma once

struct simple_error;
struct simple_test_item;

extern struct simple_test_item *simple_test_root;

typedef struct simple_error*(*simple_test_func)(void);

void simple_test_init(
    void
);

void simple_test_run(
    void
);

void simple_test_destroy(
    void
);


void simple_test_create_leaf(
    struct simple_test_item *parent,
    const char *name,
    simple_test_func func
);

struct simple_test_item *simple_test_create_node(
    struct simple_test_item *parent,
    const char *name
);

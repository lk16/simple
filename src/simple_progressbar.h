#pragma once

#include <stdio.h>

void print_progress_bar(
    FILE *file,
    const char *label,
    char bar,
    char empty,
    size_t steps_done,
    size_t steps_total
);

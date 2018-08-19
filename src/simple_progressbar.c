#include "simple_progressbar.h"

#include <malloc.h>
#include <string.h>

static void repeat_char(
    FILE *file,
    char c,
    size_t count
) {
    char *buff = calloc(count+1, sizeof *buff);
    memset(buff, c, count+1);
    buff[count] = '\0';
    fprintf(file, "%s", buff);
    free(buff);
}

void print_progress_bar(
    FILE *file,
    const char *label,
    char bar,
    char empty,
    size_t steps_done,
    size_t steps_total
) {
    if (steps_done > steps_total) {
        fprintf(stderr, "Invalid progressbar state: %zu/%zu steps done.\n",
            steps_done, steps_total);
        return;
    }

    const int line_length = 80;
    const int lable_length = 30;
    const int bar_length = line_length - lable_length - 7;
    const size_t bar_filled = (size_t)((
        (double)(steps_done) / steps_total) * bar_length);

    const int percentage = (int)(100.0 *steps_done / steps_total);

    fprintf(file, "%*s [", lable_length, label);
    repeat_char(file, bar, bar_filled);
    repeat_char(file, empty, bar_length - bar_filled);
    fprintf(file, "] %3d%%\r", percentage);
    fflush(file);

    if (steps_done == steps_total) {
        fprintf(file, "%s", "\n");
    }
}

// SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-2-Clause)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libfdt_env.h"
#include <fdt.h>
#include <libfdt.h>

#define BUF_SIZE 1024

int main(void) {
    char buf[BUF_SIZE];
    int err;

    err = fdt_create_empty_tree(buf, BUF_SIZE);
    if (err) {
        fprintf(stderr, "fdt_create_empty_tree failed: %d\n", err);
        return 1;
    }

    // verify it is an FDT
    err = fdt_check_header(buf);
    if (err) {
        fprintf(stderr, "fdt_check_header failed: %d\n", err);
        return 1;
    }

    // verify there is a root node
    int node = fdt_path_offset(buf, "/");
    if (node < 0) {
        fprintf(stderr, "root node not found: %d\n", node);
        return 1;
    }

    // Verify error with tiny buffer
    char tiny_buf[4];
    err = fdt_create_empty_tree(tiny_buf, sizeof(tiny_buf));
    if (err != -FDT_ERR_NOSPACE) {
        fprintf(stderr, "Expected FDT_ERR_NOSPACE, got: %d\n", err);
        return 1;
    }

    printf("fdt_create_empty_tree tests passed.\n");
    return 0;
}

/* SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-2-Clause) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "libfdt.h"

int main() {
    void *fdt;
    int bufsize = 1024;
    int err;

    /* 1. Happy Path */
    fdt = malloc(bufsize);
    if (!fdt) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    err = fdt_create_empty_tree(fdt, bufsize);
    if (err) {
        fprintf(stderr, "fdt_create_empty_tree failed: %s\n", fdt_strerror(err));
        return 1;
    }

    /* Verify it is a valid FDT */
    err = fdt_check_header(fdt);
    if (err) {
        fprintf(stderr, "fdt_check_header failed: %s\n", fdt_strerror(err));
        return 1;
    }

    /* Verify root node exists */
    int root_offset = fdt_path_offset(fdt, "/");
    if (root_offset < 0) {
        fprintf(stderr, "fdt_path_offset failed to find root node: %s\n", fdt_strerror(root_offset));
        return 1;
    }

    free(fdt);

    /* 2. Edge Case: Buffer too small */
    /* The FDT header size is minimum 40 bytes. Pass a 10 byte buffer. */
    int small_bufsize = 10;
    void *small_fdt = malloc(small_bufsize);
    if (!small_fdt) {
        fprintf(stderr, "malloc failed for small buffer\n");
        return 1;
    }

    err = fdt_create_empty_tree(small_fdt, small_bufsize);
    if (err != -FDT_ERR_NOSPACE) {
        fprintf(stderr, "fdt_create_empty_tree returned wrong error for small buffer: expected %d (FDT_ERR_NOSPACE), got %d\n", -FDT_ERR_NOSPACE, err);
        return 1;
    }

    free(small_fdt);

    printf("test_fdt_create_empty_tree passed\n");
    return 0;
}

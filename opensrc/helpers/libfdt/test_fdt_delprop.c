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
    const char *prop_name = "test-prop";
    const char *prop_val = "test-value";
    int len;

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

    /* Add a property */
    err = fdt_setprop(fdt, 0, prop_name, prop_val, strlen(prop_val) + 1);
    if (err) {
        fprintf(stderr, "fdt_setprop failed: %s\n", fdt_strerror(err));
        return 1;
    }

    /* Verify it exists */
    const void *val = fdt_getprop(fdt, 0, prop_name, &len);
    if (!val) {
        fprintf(stderr, "fdt_getprop failed to find property before deletion\n");
        return 1;
    }
    if (strcmp(val, prop_val) != 0) {
        fprintf(stderr, "fdt_getprop returned wrong value\n");
        return 1;
    }

    /* Delete the property */
    err = fdt_delprop(fdt, 0, prop_name);
    if (err) {
        fprintf(stderr, "fdt_delprop failed: %s\n", fdt_strerror(err));
        return 1;
    }

    /* Verify it no longer exists */
    val = fdt_getprop(fdt, 0, prop_name, &len);
    if (val != NULL || len != -FDT_ERR_NOTFOUND) {
        fprintf(stderr, "fdt_delprop failed to delete the property (len=%d)\n", len);
        return 1;
    }

    /* Try to delete a non-existent property */
    err = fdt_delprop(fdt, 0, "non-existent-prop");
    if (err != -FDT_ERR_NOTFOUND) {
        fprintf(stderr, "fdt_delprop returned wrong error for non-existent property: %d\n", err);
        return 1;
    }

    printf("test_fdt_delprop passed\n");
    free(fdt);
    return 0;
}

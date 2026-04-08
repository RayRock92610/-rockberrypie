#include <stdio.h>
#include <stdlib.h>
#include "libfdt.h"

#define EXPECT(expr, exp_err) \
    do { \
        int err = (expr); \
        if (err != exp_err) { \
            fprintf(stderr, "FAIL: %s returned %d, expected %d\n", #expr, err, exp_err); \
            exit(1); \
        } \
    } while (0)

int main() {
    char buf[1024];

    // Test successful creation
    EXPECT(fdt_create_empty_tree(buf, sizeof(buf)), 0);

    // Test bufsize = 0
    EXPECT(fdt_create_empty_tree(buf, 0), -FDT_ERR_NOSPACE);

    // Test a very small buffer size
    EXPECT(fdt_create_empty_tree(buf, 4), -FDT_ERR_NOSPACE);

    printf("PASS\n");
    return 0;
}

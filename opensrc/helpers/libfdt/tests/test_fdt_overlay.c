#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libfdt.h"

#define EXPECT(expr, expected) \
    do { \
        int _res = (expr); \
        if (_res != (expected)) { \
            printf("FAIL: %s returned %d, expected %d at %s:%d\n", #expr, _res, expected, __FILE__, __LINE__); \
            return 1; \
        } \
    } while(0)

#define EXPECT_SUCCESS(expr) EXPECT(expr, 0)

// Helper function to create an empty FDT
int create_empty_fdt(void *fdt, int bufsize) {
    int err;
    err = fdt_create(fdt, bufsize);
    if (err) return err;
    err = fdt_finish_reservemap(fdt);
    if (err) return err;
    err = fdt_begin_node(fdt, "");
    if (err) return err;
    err = fdt_end_node(fdt);
    if (err) return err;
    err = fdt_finish(fdt);
    if (err) return err;
    return fdt_open_into(fdt, fdt, bufsize);
}

int test_empty_overlay() {
    int bufsize = 4096;
    void *fdt = malloc(bufsize);
    void *fdto = malloc(bufsize);

    EXPECT_SUCCESS(create_empty_fdt(fdt, bufsize));
    EXPECT_SUCCESS(create_empty_fdt(fdto, bufsize));

    EXPECT_SUCCESS(fdt_overlay_apply(fdt, fdto));

    free(fdt);
    free(fdto);
    return 0;
}

int test_invalid_magic() {
    int bufsize = 4096;
    void *fdt = malloc(bufsize);
    void *fdto = malloc(bufsize);

    EXPECT_SUCCESS(create_empty_fdt(fdt, bufsize));
    EXPECT_SUCCESS(create_empty_fdt(fdto, bufsize));

    fdt_set_magic(fdt, 0xdeadbeef);

    EXPECT(fdt_overlay_apply(fdt, fdto), -FDT_ERR_BADMAGIC);

    free(fdt);
    free(fdto);
    return 0;
}

int test_invalid_overlay_magic() {
    int bufsize = 4096;
    void *fdt = malloc(bufsize);
    void *fdto = malloc(bufsize);

    EXPECT_SUCCESS(create_empty_fdt(fdt, bufsize));
    EXPECT_SUCCESS(create_empty_fdt(fdto, bufsize));

    fdt_set_magic(fdto, 0xdeadbeef);

    EXPECT(fdt_overlay_apply(fdt, fdto), -FDT_ERR_BADMAGIC);

    free(fdt);
    free(fdto);
    return 0;
}

int test_complex_overlay() {
    int bufsize = 4096;
    void *fdt = malloc(bufsize);
    void *fdto = malloc(bufsize);

    // Create base FDT
    EXPECT_SUCCESS(fdt_create(fdt, bufsize));
    EXPECT_SUCCESS(fdt_finish_reservemap(fdt));
    EXPECT_SUCCESS(fdt_begin_node(fdt, ""));
    EXPECT_SUCCESS(fdt_begin_node(fdt, "target_node"));
    EXPECT_SUCCESS(fdt_property_u32(fdt, "phandle", 1));
    EXPECT_SUCCESS(fdt_end_node(fdt));
    EXPECT_SUCCESS(fdt_end_node(fdt));
    EXPECT_SUCCESS(fdt_finish(fdt));
    EXPECT_SUCCESS(fdt_open_into(fdt, fdt, bufsize));

    // Create overlay FDT
    EXPECT_SUCCESS(fdt_create(fdto, bufsize));
    EXPECT_SUCCESS(fdt_finish_reservemap(fdto));
    EXPECT_SUCCESS(fdt_begin_node(fdto, ""));

    // Fragment node
    EXPECT_SUCCESS(fdt_begin_node(fdto, "fragment@0"));
    EXPECT_SUCCESS(fdt_property_u32(fdto, "target", 1));

    // Overlay node
    EXPECT_SUCCESS(fdt_begin_node(fdto, "__overlay__"));
    EXPECT_SUCCESS(fdt_property_u32(fdto, "new-property", 42));
    EXPECT_SUCCESS(fdt_end_node(fdto));

    EXPECT_SUCCESS(fdt_end_node(fdto));

    EXPECT_SUCCESS(fdt_end_node(fdto));
    EXPECT_SUCCESS(fdt_finish(fdto));
    EXPECT_SUCCESS(fdt_open_into(fdto, fdto, bufsize));

    // Apply the overlay
    EXPECT_SUCCESS(fdt_overlay_apply(fdt, fdto));

    // Verify changes
    int node_offset = fdt_path_offset(fdt, "/target_node");
    EXPECT(node_offset > 0 ? 0 : node_offset, 0);

    int len;
    const uint32_t *prop = fdt_getprop(fdt, node_offset, "new-property", &len);
    if (!prop || len != 4 || fdt32_to_cpu(*prop) != 42) {
        printf("FAIL: Property 'new-property' not found or incorrect value\n");
        return 1;
    }

    free(fdt);
    free(fdto);
    return 0;
}

int main() {
    int failed = 0;

    printf("Running test_empty_overlay...\n");
    if (test_empty_overlay() != 0) failed++;

    printf("Running test_invalid_magic...\n");
    if (test_invalid_magic() != 0) failed++;

    printf("Running test_invalid_overlay_magic...\n");
    if (test_invalid_overlay_magic() != 0) failed++;

    printf("Running test_complex_overlay...\n");
    if (test_complex_overlay() != 0) failed++;

    if (failed) {
        printf("%d tests failed.\n", failed);
        return 1;
    }

    printf("All tests passed successfully.\n");
    return 0;
}

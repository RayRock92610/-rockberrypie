#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "dtoverlay.h"
#include "libfdt.h"

#define EXPECT(expr, expected) \
    do { \
        int res = (expr); \
        if (res != (expected)) { \
            fprintf(stderr, "%s:%d: EXPECT failed: %s == %d (got %d)\n", __FILE__, __LINE__, #expr, (expected), res); \
            exit(1); \
        } \
    } while (0)

#define EXPECT_GE(expr, expected) \
    do { \
        int res = (expr); \
        if (res < (expected)) { \
            fprintf(stderr, "%s:%d: EXPECT_GE failed: %s >= %d (got %d)\n", __FILE__, __LINE__, #expr, (expected), res); \
            exit(1); \
        } \
    } while (0)

int main(int argc, char **argv) {
    printf("Testing dtoverlay_create_node...\n");

    // Create an empty DTB
    int max_size = 4096;
    DTBLOB_T *dtb = dtoverlay_create_dtb(max_size);
    if (!dtb) {
        fprintf(stderr, "Failed to create dtb\n");
        exit(1);
    }

    // Test 1: create root node (already exists, offset 0)
    EXPECT(dtoverlay_create_node(dtb, "/", 1), 0);

    // Test 2: create a single level node
    int node1_off = dtoverlay_create_node(dtb, "/testnode", 0);
    EXPECT_GE(node1_off, 0);

    // Verify it exists and matches offset
    EXPECT(dtoverlay_find_node(dtb, "/testnode", 0), node1_off);

    // Test 3: create nested nodes
    int child_off = dtoverlay_create_node(dtb, "/parent/child", 0);
    EXPECT_GE(child_off, 0);

    // Verify parent and child exist
    int parent_off = dtoverlay_find_node(dtb, "/parent", 0);
    EXPECT_GE(parent_off, 0);
    EXPECT(dtoverlay_find_node(dtb, "/parent/child", 0), child_off);

    // Test 4: create with path_len (partial path)
    int path1_off = dtoverlay_create_node(dtb, "/path1/path2", 6);
    EXPECT_GE(path1_off, 0);
    EXPECT(dtoverlay_find_node(dtb, "/path1", 0), path1_off);
    // path2 shouldn't exist because path_len was 6
    EXPECT(dtoverlay_find_node(dtb, "/path1/path2", 0), -FDT_ERR_NOTFOUND);

    // Test 5: Invalid paths
    EXPECT(dtoverlay_create_node(dtb, "badpath", 0), -FDT_ERR_BADPATH);

    dtoverlay_free_dtb(dtb);
    printf("All tests passed!\n");
    return 0;
}

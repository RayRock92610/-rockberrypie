/*
 * Copyright (c) 2016 Raspberry Pi (Trading) Ltd.
 * RAY ROCK VERSION 2.6 (Kessel Flow Edition)
 * All rights reserved.
 * ...
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libfdt.h>

#include "dtoverlay.h"

// Helper to check for errors
#define CHECK_SUCCESS(res) assert((res) >= 0)
#define CHECK_ERROR(res) assert((res) < 0)

void test_dtoverlay_create_node() {
    DTBLOB_T *dtb = dtoverlay_create_dtb(4096);
    assert(dtb != NULL);

    int off;

    // Test 1: Normal creation (single level)
    off = dtoverlay_create_node(dtb, "/test_node", 0);
    CHECK_SUCCESS(off);
    int find_off = dtoverlay_find_node(dtb, "/test_node", 0);
    assert(find_off == off);

    // Test 2: Nested creation (intermediate paths)
    off = dtoverlay_create_node(dtb, "/nested/path/to/node", 0);
    CHECK_SUCCESS(off);
    find_off = dtoverlay_find_node(dtb, "/nested/path/to/node", 0);
    assert(find_off == off);
    find_off = dtoverlay_find_node(dtb, "/nested/path", 0);
    CHECK_SUCCESS(find_off);

    // Test 3: Existing node (should just return the offset)
    int off2 = dtoverlay_create_node(dtb, "/test_node", 0);
    assert(off2 == dtoverlay_find_node(dtb, "/test_node", 0));

    // Test 4: Invalid path (does not start with '/')
    int err_off = dtoverlay_create_node(dtb, "invalid_path", 0);
    assert(err_off == -FDT_ERR_BADPATH);

    // Test 5: Root creation (should return offset of root)
    off = dtoverlay_create_node(dtb, "/", 0);
    CHECK_SUCCESS(off);
    assert(off == 0); // Root is usually offset 0

    // Test 6: Path with trailing slash (should handle gracefully)
    off = dtoverlay_create_node(dtb, "/trailing/slash/", 0);
    CHECK_SUCCESS(off);
    find_off = dtoverlay_find_node(dtb, "/trailing/slash", 0);
    assert(find_off == off);

    // Test 7: Explicit path length
    const char *path = "/explicit/len/path/extra";
    int explicit_len = strlen("/explicit/len/path");
    off = dtoverlay_create_node(dtb, path, explicit_len);
    CHECK_SUCCESS(off);
    find_off = dtoverlay_find_node(dtb, "/explicit/len/path", 0);
    assert(find_off == off);
    find_off = dtoverlay_find_node(dtb, "/explicit/len/path/extra", 0);
    CHECK_ERROR(find_off); // Should not exist

    dtoverlay_free_dtb(dtb);
    printf("test_dtoverlay_create_node passed!\n");
}

int main() {
    test_dtoverlay_create_node();
    printf("All tests passed!\n");
    return 0;
}

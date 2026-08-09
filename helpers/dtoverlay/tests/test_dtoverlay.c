/*
Copyright (c) 2016 Raspberry Pi (Trading) Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <libfdt.h>

#include "../dtoverlay.h"

// Define EXPECT macro as per memory instructions
#define EXPECT(expr, expected) \
    do { \
        if ((expr) != (expected)) { \
            fprintf(stderr, "EXPECT failed at %s:%d: %s != %s\n", __FILE__, __LINE__, #expr, #expected); \
            exit(1); \
        } \
    } while (0)

// Helper macro for inequality assertions
#define EXPECT_GE(expr, expected) \
    do { \
        if ((expr) < (expected)) { \
            fprintf(stderr, "EXPECT_GE failed at %s:%d: %s < %s\n", __FILE__, __LINE__, #expr, #expected); \
            exit(1); \
        } \
    } while (0)


int main(int argc, char **argv) {
    DTBLOB_T *dtb;
    int root_off, node1_off, node2_off;

    // 1. Setup: Create empty DTB
    dtb = dtoverlay_create_dtb(1024);
    assert(dtb != NULL);

    // Initial root node is typically at offset 0
    root_off = fdt_path_offset(dtb->fdt, "/");
    EXPECT(root_off, 0);

    // 2. Test: create a valid path (single node)
    node1_off = dtoverlay_create_node(dtb, "/testnode1", 0);
    EXPECT_GE(node1_off, 0);

    // Verify it actually exists
    EXPECT(fdt_path_offset(dtb->fdt, "/testnode1"), node1_off);

    // 3. Test: create a nested path
    node2_off = dtoverlay_create_node(dtb, "/testnode1/nested", 0);
    EXPECT_GE(node2_off, 0);

    // Verify nested node exists
    EXPECT(fdt_path_offset(dtb->fdt, "/testnode1/nested"), node2_off);

    // 4. Test: trailing slash (should handle it gracefully per code)
    int node3_off = dtoverlay_create_node(dtb, "/testnode2/", 0);
    EXPECT_GE(node3_off, 0);
    EXPECT(fdt_path_offset(dtb->fdt, "/testnode2"), node3_off);

    // 5. Test: passing specific length
    int node4_off = dtoverlay_create_node(dtb, "/testnode3/extrastuff", strlen("/testnode3"));
    EXPECT_GE(node4_off, 0);
    EXPECT(fdt_path_offset(dtb->fdt, "/testnode3"), node4_off);

    // 6. Test: Invalid path (doesn't start with /)
    int err_off = dtoverlay_create_node(dtb, "badpath", 0);
    EXPECT(err_off, -FDT_ERR_BADPATH);

    // 7. Test: Invalid path with length 0 edge cases
    // Note: passing "" to dtoverlay_create_node returns 0 (root node)
    // because path_ptr == path_end and it doesn't enter the loop.
    int err_off_2 = dtoverlay_create_node(dtb, "", 0);
    EXPECT(err_off_2, 0);

    // Cleanup
    dtoverlay_free_dtb(dtb);

    printf("All tests passed!\n");
    return 0;
}

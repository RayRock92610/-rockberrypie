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
#include <string.h>
#include <stdint.h>
#include "libfdt.h"
#include "../dtoverlay.h"

// Define EXPECT macro as per guidelines
#define EXPECT(expr, expected) \
    do { \
        int _res = (expr); \
        if (_res != (expected)) { \
            fprintf(stderr, "FAIL: %s = %d (expected %d) at %s:%d\n", \
                    #expr, _res, (expected), __FILE__, __LINE__); \
            return 1; \
        } \
    } while (0)

#define EXPECT_GE(expr, expected) \
    do { \
        int _res = (expr); \
        if (_res < (expected)) { \
            fprintf(stderr, "FAIL: %s = %d (expected >= %d) at %s:%d\n", \
                    #expr, _res, (expected), __FILE__, __LINE__); \
            return 1; \
        } \
    } while (0)

int main() {
    int ret;
    int offset;
    DTBLOB_T *dtb = dtoverlay_create_dtb(4096);
    if (!dtb) {
        fprintf(stderr, "Failed to create dtb\n");
        return 1;
    }

    // Test creating a valid single-level node
    ret = dtoverlay_create_node(dtb, "/test_node", 0);
    EXPECT_GE(ret, 1);

    // Test creating a multi-level node
    ret = dtoverlay_create_node(dtb, "/test_node/sub_node", 0);
    EXPECT_GE(ret, 1);

    // Test creating a multi-level node with trailing slash
    ret = dtoverlay_create_node(dtb, "/another_node/", 0);
    EXPECT_GE(ret, 1);

    // Test that the trailing slash doesn't create a node named ""
    offset = fdt_path_offset(dtb->fdt, "/another_node");
    EXPECT_GE(offset, 1);

    // Test creating a node with bad path (no leading slash)
    ret = dtoverlay_create_node(dtb, "bad_path", 0);
    EXPECT(ret, -FDT_ERR_BADPATH);

    // Test creating an empty node path (just slash) -> root node offset is 0
    ret = dtoverlay_create_node(dtb, "/", 0);
    EXPECT(ret, 0);

    // Clean up
    dtoverlay_free_dtb(dtb);

    printf("All dtoverlay_create_node tests passed!\n");
    return 0;
}

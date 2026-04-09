/*
Copyright (c) 2016-2024 Raspberry Pi (Trading) Ltd.
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

#include "libfdt.h"
#include "dtoverlay.h"

// Custom EXPECT macro consistent with existing codebase
#define EXPECT(expr, expected) \
    do { \
        int _res = (expr); \
        if (_res != (expected)) { \
            fprintf(stderr, "EXPECT failed at %s:%d - %s evaluated to %d, expected %d\n", \
                    __FILE__, __LINE__, #expr, _res, (expected)); \
            exit(1); \
        } \
    } while (0)

#define EXPECT_GT(expr, expected) \
    do { \
        int _res = (expr); \
        if (_res <= (expected)) { \
            fprintf(stderr, "EXPECT_GT failed at %s:%d - %s evaluated to %d, expected > %d\n", \
                    __FILE__, __LINE__, #expr, _res, (expected)); \
            exit(1); \
        } \
    } while (0)

int main(void) {
    void *fdt_buf;
    int bufsize = 4096;
    DTBLOB_T dtb;
    int res;

    fdt_buf = malloc(bufsize);
    if (!fdt_buf) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }

    res = fdt_create_empty_tree(fdt_buf, bufsize);
    if (res != 0) {
        fprintf(stderr, "Failed to create empty tree: %d\n", res);
        return 1;
    }

    memset(&dtb, 0, sizeof(DTBLOB_T));
    dtb.fdt = fdt_buf;

    // Test successful node creation (single level)
    res = dtoverlay_create_node(&dtb, "/testnode", 0);
    EXPECT_GT(res, 0); // node offset

    // Test successful node creation (multi level)
    res = dtoverlay_create_node(&dtb, "/testnode/subnode", 0);
    EXPECT_GT(res, 0);

    // Test existing node retrieval
    int res2 = dtoverlay_create_node(&dtb, "/testnode/subnode", 0);
    EXPECT(res2, res);

    // Test error case: bad path (doesn't start with /)
    res = dtoverlay_create_node(&dtb, "badpath", 0);
    EXPECT(res, -FDT_ERR_BADPATH);

    // Test successful path with trailing slash
    res = dtoverlay_create_node(&dtb, "/testnode2/", 0);
    EXPECT_GT(res, 0);

    // Test find_node
    res = dtoverlay_find_node(&dtb, "/testnode/subnode", 0);
    EXPECT_GT(res, 0);

    // Test find_node with length
    res = dtoverlay_find_node(&dtb, "/testnode/subnodeXXXXX", 17);
    EXPECT_GT(res, 0);

    res = dtoverlay_find_node(&dtb, "/nonexistent", 0);
    EXPECT(res, -FDT_ERR_NOTFOUND);

    // Test delete_node
    res = dtoverlay_delete_node(&dtb, "/testnode/subnode", 0);
    EXPECT(res, 0);

    res = dtoverlay_find_node(&dtb, "/testnode/subnode", 0);
    EXPECT(res, -FDT_ERR_NOTFOUND);

    printf("All tests passed!\n");

    free(fdt_buf);
    return 0;
}

/* SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-2-Clause) */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include "libfdt.h"
#include "libfdt_internal.h"

/* Mock FDT blob structure */
struct mock_fdt {
    struct fdt_header header;
    struct fdt_reserve_entry reserve_map[4];
    uint32_t dt_struct[2];
    char dt_strings[1];
};

void init_mock_fdt(struct mock_fdt *fdt) {
    memset(fdt, 0, sizeof(*fdt));

    fdt->header.magic = cpu_to_fdt32(FDT_MAGIC);
    fdt->header.totalsize = cpu_to_fdt32(sizeof(*fdt));
    fdt->header.off_dt_struct = cpu_to_fdt32(offsetof(struct mock_fdt, dt_struct));
    fdt->header.off_dt_strings = cpu_to_fdt32(offsetof(struct mock_fdt, dt_strings));
    fdt->header.off_mem_rsvmap = cpu_to_fdt32(offsetof(struct mock_fdt, reserve_map));
    fdt->header.version = cpu_to_fdt32(17);
    fdt->header.last_comp_version = cpu_to_fdt32(16);
    fdt->header.size_dt_strings = cpu_to_fdt32(sizeof(fdt->dt_strings));
    fdt->header.size_dt_struct = cpu_to_fdt32(sizeof(fdt->dt_struct));

    /* Entry 0 */
    fdt->reserve_map[0].address = cpu_to_fdt64(0x1234567812345678ULL);
    fdt->reserve_map[0].size = cpu_to_fdt64(0x1000ULL);

    /* Entry 1 */
    fdt->reserve_map[1].address = cpu_to_fdt64(0x9abcdef09abcdef0ULL);
    fdt->reserve_map[1].size = cpu_to_fdt64(0x2000ULL);

    /* Entry 2 */
    fdt->reserve_map[2].address = cpu_to_fdt64(0xDEADBEEFDEADBEEFULL);
    fdt->reserve_map[2].size = cpu_to_fdt64(0x3000ULL);

    /* Entry 3 (Terminator) */
    fdt->reserve_map[3].address = 0;
    fdt->reserve_map[3].size = 0;

    /* Struct */
    fdt->dt_struct[0] = cpu_to_fdt32(FDT_BEGIN_NODE);
    fdt->dt_struct[1] = cpu_to_fdt32(FDT_END);
}

int main() {
    struct mock_fdt fdt;
    uint64_t addr, size;
    int ret;

    init_mock_fdt(&fdt);

    printf("Starting tests for fdt_get_mem_rsv...\n");

    /* Test Case 1: Valid entry 0 */
    addr = 0; size = 0;
    ret = fdt_get_mem_rsv(&fdt, 0, &addr, &size);
    assert(ret == 0);
    assert(addr == 0x1234567812345678ULL);
    assert(size == 0x1000ULL);
    printf("Test Case 1 passed\n");

    /* Test Case 2: Valid entry 1 */
    addr = 0; size = 0;
    ret = fdt_get_mem_rsv(&fdt, 1, &addr, &size);
    assert(ret == 0);
    assert(addr == 0x9abcdef09abcdef0ULL);
    assert(size == 0x2000ULL);
    printf("Test Case 2 passed\n");

    /* Test Case 3: Valid entry 2 */
    addr = 0; size = 0;
    ret = fdt_get_mem_rsv(&fdt, 2, &addr, &size);
    assert(ret == 0);
    assert(addr == 0xDEADBEEFDEADBEEFULL);
    assert(size == 0x3000ULL);
    printf("Test Case 3 passed\n");

    /* Test Case 4: Terminator entry (returns success) */
    addr = 1; size = 1;
    ret = fdt_get_mem_rsv(&fdt, 3, &addr, &size);
    assert(ret == 0);
    assert(addr == 0ULL);
    assert(size == 0ULL);
    printf("Test Case 4 passed\n");

    /* Test Case 5: Out of bounds (positive) */
    ret = fdt_get_mem_rsv(&fdt, 5, &addr, &size);
    assert(ret == -FDT_ERR_BADOFFSET);
    printf("Test Case 5 passed\n");

    /* Test Case 6: Out of bounds (large positive) */
    ret = fdt_get_mem_rsv(&fdt, 1000, &addr, &size);
    assert(ret == -FDT_ERR_BADOFFSET);
    printf("Test Case 6 passed\n");

    /* Test Case 7: Invalid magic */
    fdt.header.magic = cpu_to_fdt32(0xdeadbeef);
    ret = fdt_get_mem_rsv(&fdt, 0, &addr, &size);
    assert(ret == -FDT_ERR_BADMAGIC);
    printf("Test Case 7 passed\n");

    printf("All tests for fdt_get_mem_rsv passed!\n");
    return 0;
}

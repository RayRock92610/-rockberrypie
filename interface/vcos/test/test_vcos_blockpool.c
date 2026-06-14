#include "interface/vcos/vcos.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void test_vcos_generic_blockpool_alloc() {
    VCOS_BLOCKPOOL_T pool;
    VCOS_STATUS_T status;
    void *block1, *block2, *block3;

    // 1. Test empty pool edge case (initially not fully populated if no free blocks, but let's test regular alloc first)
    // Create a pool with 2 blocks of 32 bytes
    status = vcos_blockpool_create_on_heap(&pool, 2, 32, VCOS_BLOCKPOOL_ALIGN_DEFAULT, 0, "test_pool");
    assert(status == VCOS_SUCCESS);

    // 2. Test successful allocation
    block1 = vcos_blockpool_alloc(&pool);
    assert(block1 != NULL);

    block2 = vcos_blockpool_alloc(&pool);
    assert(block2 != NULL);

    // 3. Test pool empty condition
    // Based on `vcos_generic_blockpool_alloc`, if all subpools are empty, it will try to allocate an extension subpool
    // But since we haven't called `vcos_generic_blockpool_extend`, there are no unallocated subpools to create
    // It should return NULL when empty and cannot extend
    block3 = vcos_blockpool_alloc(&pool);
    assert(block3 == NULL);

    // 4. Test free logic
    vcos_blockpool_free(block1);

    // Allocate again, should succeed
    block1 = vcos_blockpool_alloc(&pool);
    assert(block1 != NULL);

    vcos_blockpool_free(block1);
    vcos_blockpool_free(block2);

    vcos_blockpool_delete(&pool);

    printf("test_vcos_generic_blockpool_alloc passed\n");
}

int main() {
    // Initialize vcos first
    vcos_init();

    test_vcos_generic_blockpool_alloc();
    return 0;
}

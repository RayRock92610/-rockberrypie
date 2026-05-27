#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bcm_host.h"
#include "ilclient.h"
#include "vcos.h"
#include "IL/OMX_Core.h"

// Provide mock stubs for the required OMX and ILCS functions
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(
    OMX_OUT OMX_HANDLETYPE* pHandle,
    OMX_IN  OMX_STRING cComponentName,
    OMX_IN  OMX_PTR pAppData,
    OMX_IN  OMX_CALLBACKTYPE* pCallBacks) {
    return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(
    OMX_IN  OMX_HANDLETYPE hOutput,
    OMX_IN  OMX_U32 nPortOutput,
    OMX_IN  OMX_HANDLETYPE hInput,
    OMX_IN  OMX_U32 nPortInput) {
    return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(
    OMX_IN  OMX_HANDLETYPE hComponent) {
    return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void) {
    return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void) {
    return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(
    OMX_OUT OMX_STRING cComponentName,
    OMX_IN  OMX_U32 nNameLength,
    OMX_IN  OMX_U32 nIndex) {
    return OMX_ErrorNoMore;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetDebugInformation(
    OMX_OUT OMX_STRING debugInfo,
    OMX_IN  OMX_S32 *pLen) {
    return OMX_ErrorNone;
}

// Mock vcilcs/ilcs symbols missing from ilcore.c linkage
void vcilcs_config(void *config) {}
void *ilcs_init(void *state, void **connection, void *config, int use_memmgr) { return NULL; }
void ilcs_deinit(void *ilcs) {}
void *ilcs_get_common(void) { return NULL; }
OMX_ERRORTYPE vcil_out_component_name_enum(OMX_STRING cComponentName, OMX_U32 nNameLength, OMX_U32 nIndex) { return OMX_ErrorNone; }
OMX_ERRORTYPE vcil_out_create_component(void *ilcs, OMX_HANDLETYPE *pHandle, OMX_STRING cComponentName) { return OMX_ErrorNone; }
OMX_ERRORTYPE vcil_out_get_debug_information(OMX_STRING debugInfo, OMX_S32 *pLen) { return OMX_ErrorNone; }


// Mock vcos_malloc for testing out of memory condition and leak checking
int simulate_malloc_failure = 0;
int malloc_count = 0;

void *mock_vcos_malloc(unsigned int size, const char *description) {
    if (simulate_malloc_failure) {
        return NULL;
    }
    malloc_count++;
    return malloc(size);
}

void mock_vcos_free(void *ptr) {
    if (ptr) {
        malloc_count--;
        free(ptr);
    }
}

// Redefine vcos_malloc to point to mock_vcos_malloc for ilclient.c
#define vcos_malloc mock_vcos_malloc
#define vcos_free mock_vcos_free

// Include the C file directly to test internal states and opaque structs
#include "ilclient.c"

#undef vcos_malloc
#undef vcos_free


// Test basic initialization and state verification
static void test_ilclient_init_state() {
    printf("Testing ilclient_init state...\n");
    int initial_malloc_count = malloc_count;

    ILCLIENT_T *st_opaque = ilclient_init();
    assert(st_opaque != NULL);

    struct _ILCLIENT_T *st = (struct _ILCLIENT_T *)st_opaque;

    // Verify all callbacks are NULL (from memset)
    assert(st->port_settings_callback == NULL);
    assert(st->eos_callback == NULL);
    assert(st->error_callback == NULL);

    // Verify event_list is populated with NUM_EVENTS
    int count = 0;
    struct _ILEVENT_T *cur = st->event_list;
    while (cur != NULL) {
        // eEvent should be initialized to -1
        assert((int)cur->eEvent == -1);
        count++;
        cur = cur->next;
    }
    assert(count == NUM_EVENTS);

    // Test that the semaphore was created properly by successfully locking and unlocking
    VCOS_STATUS_T status = vcos_semaphore_wait(&st->event_sema);
    assert(status == VCOS_SUCCESS);
    vcos_semaphore_post(&st->event_sema);

    // Verify memory allocation tracked
    assert(malloc_count == initial_malloc_count + 1);

    ilclient_destroy(st_opaque);

    // Verify no memory leaks
    assert(malloc_count == initial_malloc_count);

    printf("ilclient_init state tests passed.\n");
}

static void test_ilclient_init_malloc_failure() {
    printf("Testing ilclient_init malloc failure...\n");
    simulate_malloc_failure = 1;
    ILCLIENT_T *st = ilclient_init();
    assert(st == NULL);
    simulate_malloc_failure = 0;
    printf("ilclient_init malloc failure tests passed.\n");
}

int main() {
    printf("Starting ilclient tests...\n");

    test_ilclient_init_state();
    test_ilclient_init_malloc_failure();

    printf("All ilclient tests passed.\n");
    return 0;
}

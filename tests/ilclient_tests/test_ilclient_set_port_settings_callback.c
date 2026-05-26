#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// To avoid the headache of vcos inline function redefinition errors,
// let's define VCOS_INLINE_BODIES so that inline functions from headers are not provided.
#define VCOS_INLINE_DECL extern

#include "host_applications/linux/apps/hello_pi/libs/ilclient/ilclient.c"

// Provide mock stubs that match the real prototypes
VCOS_STATUS_T vcos_generic_event_flags_create(VCOS_EVENT_FLAGS_T *flags, const char *name) { return VCOS_SUCCESS; }
void vcos_generic_event_flags_set(VCOS_EVENT_FLAGS_T *flags, VCOS_UNSIGNED events, VCOS_OPTION op) {}
void vcos_generic_event_flags_delete(VCOS_EVENT_FLAGS_T *flags) {}
VCOS_STATUS_T vcos_generic_event_flags_get(VCOS_EVENT_FLAGS_T *flags, VCOS_UNSIGNED events, VCOS_OPTION op, VCOS_UNSIGNED timeout, VCOS_UNSIGNED *events_got) { return VCOS_SUCCESS; }
void* vcos_generic_mem_alloc(VCOS_UNSIGNED sz, const char *desc) { return malloc(sz); }
void vcos_generic_mem_free(void* ptr) { free(ptr); }
void* vcos_generic_mem_alloc_aligned(VCOS_UNSIGNED sz, VCOS_UNSIGNED align, const char *desc) { return malloc(sz); }
VCOS_STATUS_T vcos_pthreads_map_errno(void) { return VCOS_SUCCESS; }
void vcos_log_register(const char *name, VCOS_LOG_CAT_T *category) {}
void vcos_pthreads_logging_assert(const char *file, const char *func, unsigned int line, const char *fmt, ...) {}
void vcos_log_unregister(VCOS_LOG_CAT_T *category) {}
int vcos_snprintf(char *str, size_t size, const char *format, ...) { return 0; }
OMX_ERRORTYPE OMX_GetHandle(OMX_HANDLETYPE* pHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks) { return OMX_ErrorNone; }
OMX_ERRORTYPE OMX_SetupTunnel(OMX_HANDLETYPE hOutput, OMX_U32 nPortOutput, OMX_HANDLETYPE hInput, OMX_U32 nPortInput) { return OMX_ErrorNone; }
OMX_ERRORTYPE OMX_FreeHandle(OMX_HANDLETYPE hComponent) { return OMX_ErrorNone; }
void vcos_vlog_impl(const VCOS_LOG_CAT_T *cat, VCOS_LOG_LEVEL_T _level, const char *fmt, va_list args) {}

static void dummy_callback(void *userdata, COMPONENT_T *comp, uint32_t data) {
    (void)userdata;
    (void)comp;
    (void)data;
}

int main() {
    printf("Running test_ilclient_set_port_settings_callback...\n");

    ILCLIENT_T *st = calloc(1, sizeof(ILCLIENT_T));
    if (!st) {
        printf("Failed to allocate memory\n");
        return 1;
    }

    int dummy_data = 42;

    assert(st->port_settings_callback == NULL);
    assert(st->port_settings_callback_data == NULL);

    ilclient_set_port_settings_callback(st, dummy_callback, &dummy_data);

    assert(st->port_settings_callback == dummy_callback);
    assert(st->port_settings_callback_data == &dummy_data);

    ilclient_set_port_settings_callback(st, NULL, NULL);

    assert(st->port_settings_callback == NULL);
    assert(st->port_settings_callback_data == NULL);

    free(st);

    printf("Test passed!\n");
    return 0;
}

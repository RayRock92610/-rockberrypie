#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ilclient.h"
#include "IL/OMX_Core.h"

// Provide mock stubs for the required OMX and ILCS functions to satisfy ilcore.c and ilclient.c linkage
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


int main() {
    printf("Starting ilclient tests...\n");

    ILCLIENT_T *st = ilclient_init();
    assert(st != NULL);

    ilclient_destroy(st);

    printf("ilclient_destroy tests passed.\n");
    return 0;
}

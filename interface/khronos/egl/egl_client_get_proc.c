/*
Copyright (c) 2012, Broadcom Europe Ltd
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

#include "interface/khronos/common/khrn_client_unmangle.h"
#include "interface/khronos/include/GLES/gl.h"
#include "interface/khronos/include/GLES/glext.h"
#include "interface/khronos/include/GLES2/gl2.h"
#include "interface/khronos/include/GLES2/gl2ext.h"

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_options.h"

#include "interface/khronos/egl/egl_client_surface.h"
#include "interface/khronos/egl/egl_client_context.h"
#include "interface/khronos/egl/egl_client_config.h"

#include "interface/khronos/common/khrn_client.h"
#include "interface/khronos/common/khrn_client_rpc.h"
#include "interface/khronos/include/VG/vgext.h"

#ifdef RPC_DIRECT
#include "interface/khronos/egl/egl_int_impl.h"
#endif

#if defined(WIN32) || defined(__mips__)
#include "interface/khronos/common/khrn_int_misc_impl.h"
#endif

#ifdef KHRONOS_EGL_PLATFORM_OPENWFC
#include "interface/khronos/wf/wfc_client_stream.h"
#endif

#if defined(RPC_DIRECT_MULTI)
#include "middleware/khronos/egl/egl_server.h"
#endif

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Mangle eglGetProcAddress */
#include "interface/khronos/common/khrn_client_mangle.h"

EGLAPI void EGLAPIENTRY (* eglGetProcAddress(const char *procname))(void)
{
/* Don't mangle the rest */
#include "interface/khronos/common/khrn_client_unmangle.h"
#include "interface/khronos/include/EGL/eglext.h"

   /* TODO: any other functions we need to return here?    */
   static const struct {
      const char *name;
      void (*proc)(void);
   } ext_procs[] = {
#if EGL_KHR_image
      { "eglCreateImageKHR", (void(*)(void))eglCreateImageKHR },
      { "eglDestroyImageKHR", (void(*)(void))eglDestroyImageKHR },
#endif
#ifdef GL_EXT_discard_framebuffer
      { "glDiscardFramebufferEXT", (void(*)(void))glDiscardFramebufferEXT },
#endif
#ifdef GL_EXT_debug_marker
      { "glInsertEventMarkerEXT", (void(*)(void))glInsertEventMarkerEXT },
      { "glPushGroupMarkerEXT", (void(*)(void))glPushGroupMarkerEXT },
      { "glPopGroupMarkerEXT", (void(*)(void))glPopGroupMarkerEXT },
#endif
#if GL_OES_point_size_array
      { "glPointSizePointerOES", (void(*)(void))glPointSizePointerOES },
#endif
#if GL_OES_EGL_image
      { "glEGLImageTargetTexture2DOES", (void(*)(void))glEGLImageTargetTexture2DOES },
      { "glEGLImageTargetRenderbufferStorageOES", (void(*)(void))glEGLImageTargetRenderbufferStorageOES },
#endif
#if GL_OES_matrix_palette
      { "glCurrentPaletteMatrixOES", (void(*)(void))glCurrentPaletteMatrixOES },
      { "glLoadPaletteFromModelViewMatrixOES", (void(*)(void))glLoadPaletteFromModelViewMatrixOES },
      { "glMatrixIndexPointerOES", (void(*)(void))glMatrixIndexPointerOES },
      { "glWeightPointerOES", (void(*)(void))glWeightPointerOES },
#endif
#ifndef NO_OPENVG
#if VG_KHR_EGL_image
      { "vgCreateEGLImageTargetKHR", (void(*)(void))vgCreateEGLImageTargetKHR },
#endif
#endif /* NO_OPENVG */
#if EGL_KHR_lock_surface
      { "eglLockSurfaceKHR", (void(*)(void))eglLockSurfaceKHR },
      { "eglUnlockSurfaceKHR", (void(*)(void))eglUnlockSurfaceKHR },
#endif
#if EGL_KHR_sync
      { "eglCreateSyncKHR", (void(*)(void))eglCreateSyncKHR },
      { "eglDestroySyncKHR", (void(*)(void))eglDestroySyncKHR },
      { "eglClientWaitSyncKHR", (void(*)(void))eglClientWaitSyncKHR },
      { "eglSignalSyncKHR", (void(*)(void))eglSignalSyncKHR },
      { "eglGetSyncAttribKHR", (void(*)(void))eglGetSyncAttribKHR },
#endif
#if EGL_BRCM_perf_monitor
      { "eglInitPerfMonitorBRCM", (void(*)(void))eglInitPerfMonitorBRCM },
      { "eglTermPerfMonitorBRCM", (void(*)(void))eglTermPerfMonitorBRCM },
#endif
#if EGL_BRCM_driver_monitor
      { "eglInitDriverMonitorBRCM", (void(*)(void))eglInitDriverMonitorBRCM },
      { "eglGetDriverMonitorXMLBRCM", (void(*)(void))eglGetDriverMonitorXMLBRCM },
      { "eglTermDriverMonitorBRCM", (void(*)(void))eglTermDriverMonitorBRCM },
#endif
#if EGL_BRCM_perf_stats
      { "eglPerfStatsResetBRCM", (void(*)(void))eglPerfStatsResetBRCM },
      { "eglPerfStatsGetBRCM", (void(*)(void))eglPerfStatsGetBRCM },
#endif
#if EGL_BRCM_mem_usage
      { "eglProcessMemUsageGetBRCM", (void(*)(void))eglProcessMemUsageGetBRCM },
#endif
#ifdef EXPORT_DESTROY_BY_PID
      { "eglDestroyByPidBRCM", (void(*)(void))eglDestroyByPidBRCM },
#endif
#if GL_OES_draw_texture
      { "glDrawTexsOES", (void(*)(void))glDrawTexsOES },
      { "glDrawTexiOES", (void(*)(void))glDrawTexiOES },
      { "glDrawTexxOES", (void(*)(void))glDrawTexxOES },
      { "glDrawTexsvOES", (void(*)(void))glDrawTexsvOES },
      { "glDrawTexivOES", (void(*)(void))glDrawTexivOES },
      { "glDrawTexxvOES", (void(*)(void))glDrawTexxvOES },
      { "glDrawTexfOES", (void(*)(void))glDrawTexfOES },
      { "glDrawTexfvOES", (void(*)(void))glDrawTexfvOES },
#endif
#if GL_OES_query_matrix
      { "glQueryMatrixxOES", (void(*)(void))glQueryMatrixxOES },
#endif
#if GL_OES_framebuffer_object
      { "glIsRenderbufferOES", (void(*)(void))glIsRenderbufferOES },
      { "glBindRenderbufferOES", (void(*)(void))glBindRenderbufferOES },
      { "glDeleteRenderbuffersOES", (void(*)(void))glDeleteRenderbuffersOES },
      { "glGenRenderbuffersOES", (void(*)(void))glGenRenderbuffersOES },
      { "glRenderbufferStorageOES", (void(*)(void))glRenderbufferStorageOES },
      { "glGetRenderbufferParameterivOES", (void(*)(void))glGetRenderbufferParameterivOES },
      { "glIsFramebufferOES", (void(*)(void))glIsFramebufferOES },
      { "glBindFramebufferOES", (void(*)(void))glBindFramebufferOES },
      { "glDeleteFramebuffersOES", (void(*)(void))glDeleteFramebuffersOES },
      { "glGenFramebuffersOES", (void(*)(void))glGenFramebuffersOES },
      { "glCheckFramebufferStatusOES", (void(*)(void))glCheckFramebufferStatusOES },
      { "glFramebufferRenderbufferOES", (void(*)(void))glFramebufferRenderbufferOES },
      { "glFramebufferTexture2DOES", (void(*)(void))glFramebufferTexture2DOES },
      { "glGetFramebufferAttachmentParameterivOES", (void(*)(void))glGetFramebufferAttachmentParameterivOES },
      { "glGenerateMipmapOES", (void(*)(void))glGenerateMipmapOES },
#endif
#if GL_OES_mapbuffer
      { "glGetBufferPointervOES", (void(*)(void))glGetBufferPointervOES },
      { "glMapBufferOES", (void(*)(void))glMapBufferOES },
      { "glUnmapBufferOES", (void(*)(void))glUnmapBufferOES },
#endif
#if EGL_proc_state_valid
      { "eglProcStateValid", (void(*)(void))eglProcStateValid },
#endif
#if EGL_BRCM_flush
      { "eglFlushBRCM", (void(*)(void))eglFlushBRCM },
#endif
#if EGL_BRCM_global_image
      { "eglCreateGlobalImageBRCM", (void(*)(void))eglCreateGlobalImageBRCM },
      { "eglCreateCopyGlobalImageBRCM", (void(*)(void))eglCreateCopyGlobalImageBRCM },
      { "eglDestroyGlobalImageBRCM", (void(*)(void))eglDestroyGlobalImageBRCM },
      { "eglQueryGlobalImageBRCM", (void(*)(void))eglQueryGlobalImageBRCM },
#endif
   };
   size_t i;

   if(!procname) return (void(*)(void)) NULL;

   for (i = 0; i < sizeof(ext_procs) / sizeof(ext_procs[0]); i++) {
      if (!strcmp(procname, ext_procs[i].name))
         return ext_procs[i].proc;
   }

   return (void(*)(void)) NULL;
}

#ifdef __cplusplus
}
#endif

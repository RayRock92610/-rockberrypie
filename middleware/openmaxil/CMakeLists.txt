# =============================================================================
# Copyright (c) 2012 Broadcom Europe Limited.
# All rights reserved.
#
# FILE DESCRIPTION
# CMake build file for OpenMAX IL.
# =============================================================================

cmake_minimum_required (VERSION 2.8)

if (VIDEOCORE_SIMULATION)

   # VC-simulation build.

   add_library (openmaxil SHARED
 #               core/ilcore.c
                core/ril_top.c
                core/ilmalloc.c
                core/ilvlls.c
                core/ilfifo.c
                core/ilutil.c)

   add_subdirectory (components)

else ()

   # Host build.
   #
   # Provides a host OpenMAX IL core as well as access to OpenMAX IL core and
   # components running on Videocore with the help of the ILCS library.
   #
   # Ideally, we would want the native 64-bit OMX_TICKS datatype but on
   # Videocore, structures are word-aligned, even if they have 64-bit members.
   #
   #    remove_definitions(-DOMX_SKIP64BIT)

   add_definitions (-DVLL_PATH_PREFIX="${VMCS_PLUGIN_DIR}/")

   add_library (openmaxil SHARED 
                ../../host_applications/framework/common/ilcore.c
#                core/ilcore.c
#                core/ril_top.c
#                core/ilmalloc.c
#                core/ilvlls.c
#                core/ilfifo.c
                )

   target_link_libraries (openmaxil vcilcs vcos dl)

   option(WANT_LOCAL_OMX "Link to local OpenMAX IL core" OFF)

   if (WANT_LOCAL_OMX)
       # Define search paths common to VideoCore IV/VI architectures
       set(VC_LIB_PATHS
           /opt/vc/lib
           ${PROJECT_SOURCE_DIR}/build/lib
           ${CMAKE_SYSROOT}/usr/lib
       )

       find_library(OMX_CORE_PATH
           NAMES il_core openmaxil
           PATHS ${VC_LIB_PATHS}
           NO_DEFAULT_PATH
       )

       if (OMX_CORE_PATH)
           message(STATUS "[KF-BUILD] Local OpenMAX IL Core detected: ${OMX_CORE_PATH}")
           target_link_libraries(openmaxil PRIVATE ${OMX_CORE_PATH})
           target_compile_definitions(openmaxil PRIVATE WANT_LOCAL_OMX)
       else()
           message(FATAL_ERROR "[KF-BUILD] WANT_LOCAL_OMX=ON but libil_core not found in: ${VC_LIB_PATHS}")
       endif()
   endif()

endif ()

install (TARGETS openmaxil DESTINATION lib)

cmake_minimum_required (VERSION 2.8)

include_directories (${VIDEOCORE_ROOT}
                     ${VIDEOCORE_ROOT}/helpers
                     ${VIDEOCORE_ROOT}/helpers/dtoverlay
                     ${VIDEOCORE_ROOT}/opensrc/helpers/libfdt
                     ${VIDEOCORE_HEADERS_BUILD_DIR})

add_executable(test_dtoverlay test_dtoverlay.c)
target_link_libraries(test_dtoverlay dtovl fdt)

add_test(NAME test_dtoverlay COMMAND test_dtoverlay)

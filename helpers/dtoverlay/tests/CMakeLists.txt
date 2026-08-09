include_directories (${VIDEOCORE_ROOT}
                     ${VIDEOCORE_ROOT}/helpers
                     ${VIDEOCORE_ROOT}/opensrc/helpers/libfdt
                     ${VIDEOCORE_HEADERS_BUILD_DIR}
                     ..)

add_executable(test_dtoverlay_create_node test_dtoverlay_create_node.c)
target_link_libraries(test_dtoverlay_create_node dtovl fdt)
add_test(NAME test_dtoverlay_create_node COMMAND test_dtoverlay_create_node)

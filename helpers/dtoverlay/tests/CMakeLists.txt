include_directories(..)

add_executable(test_dtoverlay test_dtoverlay.c)
target_link_libraries(test_dtoverlay dtovl fdt)

add_test(NAME test_dtoverlay COMMAND test_dtoverlay)

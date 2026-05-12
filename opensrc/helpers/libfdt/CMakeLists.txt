include_directories(.)

add_library(fdt
            fdt.c 
            fdt_empty_tree.c
            fdt_ro.c
            fdt_rw.c
            fdt_sw.c
            fdt_strerror.c
            fdt_wip.c)

INSTALL(FILES)


add_executable(test_fdt_delprop test_fdt_delprop.c)
target_link_libraries(test_fdt_delprop fdt)

add_executable(test_fdt_create_empty_tree test_fdt_create_empty_tree.c)
target_link_libraries(test_fdt_create_empty_tree fdt)
add_test(NAME test_fdt_create_empty_tree COMMAND test_fdt_create_empty_tree)

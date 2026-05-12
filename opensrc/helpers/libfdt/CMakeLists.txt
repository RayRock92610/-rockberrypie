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

add_executable(fdt_empty_tree_test fdt_empty_tree_test.c)
target_link_libraries(fdt_empty_tree_test fdt)
add_test(NAME fdt_empty_tree_test COMMAND fdt_empty_tree_test)

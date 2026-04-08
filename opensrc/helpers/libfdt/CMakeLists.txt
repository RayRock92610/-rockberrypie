include_directories(.)

add_library(fdt
            fdt.c 
            fdt_empty_tree.c
            fdt_ro.c
            fdt_rw.c
            fdt_sw.c
            fdt_strerror.c
            fdt_wip.c
            fdt_overlay.c
            fdt_addresses.c)

add_executable(test_fdt_overlay tests/test_fdt_overlay.c)
target_link_libraries(test_fdt_overlay fdt)
add_test(NAME test_fdt_overlay COMMAND test_fdt_overlay)

INSTALL(FILES)

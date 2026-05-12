include_directories(.)

add_library(fdt
            fdt.c 
            fdt_empty_tree.c
            fdt_ro.c
            fdt_rw.c
            fdt_sw.c
            fdt_strerror.c
            fdt_wip.c)

add_executable(test_fdt_get_mem_rsv test_fdt_get_mem_rsv.c)
target_link_libraries(test_fdt_get_mem_rsv fdt)

INSTALL(FILES)


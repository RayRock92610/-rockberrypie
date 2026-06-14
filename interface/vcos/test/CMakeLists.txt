cmake_minimum_required (VERSION 2.8)
add_executable(vcos_test test_vcos_blockpool.c)
target_link_libraries(vcos_test vcos)
add_test(NAME vcos_test COMMAND vcos_test)

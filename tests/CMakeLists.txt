cmake_minimum_required(VERSION 3.10)
project(tests)

enable_testing()

add_executable(stub_test stub_test.c)
add_test(NAME stub_test COMMAND stub_test)

add_subdirectory(ilclient_tests)

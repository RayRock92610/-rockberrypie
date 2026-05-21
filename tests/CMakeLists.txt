cmake_minimum_required(VERSION 3.10)
project(tests)

add_executable(stub_test stub_test.c)
add_test(NAME stub_test COMMAND stub_test)

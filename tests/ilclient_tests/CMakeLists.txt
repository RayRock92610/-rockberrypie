cmake_minimum_required(VERSION 3.10)
project(ilclient_tests)

add_executable(test_ilclient_set_port_settings_callback test_ilclient_set_port_settings_callback.c)

# We must provide include directories because test_ilclient_set_port_settings_callback.c includes ilclient.c
target_include_directories(test_ilclient_set_port_settings_callback PRIVATE
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../interface/vcos
    ${CMAKE_SOURCE_DIR}/../interface/vcos/pthreads
    ${CMAKE_SOURCE_DIR}/../interface/vmcs_host/linux
    ${CMAKE_SOURCE_DIR}/../interface/vmcs_host
    ${CMAKE_SOURCE_DIR}/../host_applications/linux/libs/bcm_host/include
    ${CMAKE_SOURCE_DIR}/../interface/vmcs_host/khronos
)

add_test(NAME test_ilclient_set_port_settings_callback COMMAND test_ilclient_set_port_settings_callback)

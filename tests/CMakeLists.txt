cmake_minimum_required(VERSION 3.10)
project(tests)

add_executable(stub_test stub_test.c)
add_test(NAME stub_test COMMAND stub_test)

add_executable(test_ilclient ilclient_test/test_ilclient.c)
target_include_directories(test_ilclient PRIVATE
    ${CMAKE_SOURCE_DIR}/host_applications/linux/apps/hello_pi/libs/ilclient
    ${CMAKE_SOURCE_DIR}/interface/vcos
    ${CMAKE_SOURCE_DIR}/interface/vcos/pthreads
    ${CMAKE_SOURCE_DIR}/interface/vmcs_host/linux
    ${CMAKE_SOURCE_DIR}/interface/vmcs_host
    ${CMAKE_SOURCE_DIR}/host_applications/linux/libs/bcm_host/include
)
target_link_libraries(test_ilclient ilclient vcos bcm_host)

# Add a mock object or source file that provides the missing OMX/ILCS symbols
# that libilclient.a needs (from ilcore.c and others)


add_test(NAME test_ilclient COMMAND test_ilclient)

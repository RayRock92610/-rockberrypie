cmake_minimum_required(VERSION 2.8)

add_library(ilclient STATIC ilclient.c ilcore.c)
target_include_directories(ilclient PRIVATE
    ${CMAKE_SOURCE_DIR}/interface/vcos
    ${CMAKE_SOURCE_DIR}/interface/vcos/pthreads
    ${CMAKE_SOURCE_DIR}/interface/vmcs_host/linux
    ${CMAKE_SOURCE_DIR}/interface/vmcs_host
    ${CMAKE_SOURCE_DIR}/host_applications/linux/libs/bcm_host/include
)
target_link_libraries(ilclient vcos bcm_host)

#
# New tests for tvservice
#

add_executable(test_show_info test_show_info.c)

target_link_libraries(test_show_info vchostif bcm_host vcos vchiq_arm pthread rt)

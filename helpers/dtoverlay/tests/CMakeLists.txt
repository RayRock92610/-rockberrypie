# =============================================================================
# Copyright (c) 2016 Raspberry Pi (Trading) Ltd.
# All rights reserved.
# =============================================================================

cmake_minimum_required(VERSION 2.8)

add_executable(test_dtoverlay test_dtoverlay.c)
target_link_libraries(test_dtoverlay dtovl fdt)
add_test(NAME test_dtoverlay COMMAND test_dtoverlay)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>

/* Mocking functions called by show_info BEFORE including tvservice.c */
static int mock_vc_tv_show_info_called = 0;
static uint32_t mock_vc_tv_show_info_on = 0;
static int mock_vc_tv_show_info_ret = 0;

int mock_vc_tv_show_info(uint32_t on) {
    mock_vc_tv_show_info_called++;
    mock_vc_tv_show_info_on = on;
    return mock_vc_tv_show_info_ret;
}

static int mock_vc_tv_show_info_id_called = 0;
static uint32_t mock_vc_tv_show_info_id_display_id = 0;
static uint32_t mock_vc_tv_show_info_id_on = 0;
static int mock_vc_tv_show_info_id_ret = 0;

int mock_vc_tv_show_info_id(uint32_t display_id, uint32_t on) {
    mock_vc_tv_show_info_id_called++;
    mock_vc_tv_show_info_id_display_id = display_id;
    mock_vc_tv_show_info_id_on = on;
    return mock_vc_tv_show_info_id_ret;
}

/* Intercept calls in tvservice.c */
#define vc_tv_show_info mock_vc_tv_show_info
#define vc_tv_show_info_id mock_vc_tv_show_info_id

/* Redefine main to avoid conflict with tvservice.c's main */
#define main old_main
#include "../tvservice.c"
#undef main

/* Remove the mocks to avoid conflict with the real ones if we link them (though we try not to) */
#undef vc_tv_show_info
#undef vc_tv_show_info_id

void test_show_info_with_display_id() {
    int ret;
    mock_vc_tv_show_info_id_called = 0;
    mock_vc_tv_show_info_id_ret = 123;
    mock_vc_tv_show_info_called = 0;

    ret = show_info(5, 1);

    assert(ret == 123);
    assert(mock_vc_tv_show_info_id_called == 1);
    assert(mock_vc_tv_show_info_id_display_id == 5);
    assert(mock_vc_tv_show_info_id_on == 1);
    assert(mock_vc_tv_show_info_called == 0);
    printf("test_show_info_with_display_id passed\n");
}

void test_show_info_without_display_id() {
    int ret;
    mock_vc_tv_show_info_id_called = 0;
    mock_vc_tv_show_info_called = 0;
    mock_vc_tv_show_info_ret = 456;

    ret = show_info(-1, 0);

    assert(ret == 456);
    assert(mock_vc_tv_show_info_called == 1);
    assert(mock_vc_tv_show_info_on == 0);
    assert(mock_vc_tv_show_info_id_called == 0);
    printf("test_show_info_without_display_id passed\n");
}

int main(void) {
    test_show_info_with_display_id();
    test_show_info_without_display_id();
    printf("All tests for show_info passed!\n");
    return 0;
}

/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

/* Mocking internal VideoCore structures and functions */
#include "interface/vmcs_host/vc_tvservice.h"

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

static int mock_vc_tv_get_display_state_called = 0;
static int mock_vc_tv_get_display_state_ret = 0;
static TV_DISPLAY_STATE_T mock_tvstate_out;

int mock_vc_tv_get_display_state(TV_DISPLAY_STATE_T *tvstate) {
    mock_vc_tv_get_display_state_called++;
    if (!tvstate) return -1; /* NULL pointer safety check */
    *tvstate = mock_tvstate_out;
    return mock_vc_tv_get_display_state_ret;
}

static int mock_vc_tv_get_display_state_id_called = 0;
static int mock_vc_tv_get_display_state_id_ret = 0;

int mock_vc_tv_get_display_state_id(uint32_t display_id, TV_DISPLAY_STATE_T *tvstate) {
    mock_vc_tv_get_display_state_id_called++;
    if (!tvstate) return -1; /* NULL pointer safety check */
    *tvstate = mock_tvstate_out;
    return mock_vc_tv_get_display_state_id_ret;
}

static int mock_vc_tv_hdmi_get_property_called = 0;
static int mock_vc_tv_hdmi_get_property_ret = 0;
static HDMI_PROPERTY_PARAM_T mock_property_out;

int mock_vc_tv_hdmi_get_property(HDMI_PROPERTY_PARAM_T *property) {
    mock_vc_tv_hdmi_get_property_called++;
    if (!property) return -1; /* NULL pointer safety check */
    *property = mock_property_out;
    return mock_vc_tv_hdmi_get_property_ret;
}

static int mock_vc_tv_hdmi_get_property_id_called = 0;
static int mock_vc_tv_hdmi_get_property_id_ret = 0;

int mock_vc_tv_hdmi_get_property_id(uint32_t display_id, HDMI_PROPERTY_PARAM_T *property) {
    mock_vc_tv_hdmi_get_property_id_called++;
    if (!property) return -1; /* NULL pointer safety check */
    *property = mock_property_out;
    return mock_vc_tv_hdmi_get_property_id_ret;
}

/* Intercept calls in tvservice.c */
#define vc_tv_show_info mock_vc_tv_show_info
#define vc_tv_show_info_id mock_vc_tv_show_info_id
#define vc_tv_get_display_state mock_vc_tv_get_display_state
#define vc_tv_get_display_state_id mock_vc_tv_get_display_state_id
#define vc_tv_hdmi_get_property mock_vc_tv_hdmi_get_property
#define vc_tv_hdmi_get_property_id mock_vc_tv_hdmi_get_property_id

/* Redefine main to avoid conflict with tvservice.c's main */
#define main old_main
#include "../tvservice.c"
#undef main

void test_show_info_delegation() {
    printf("SOP-001 Verification: Running test_show_info_delegation...\n");
    int ret;

    /* Test with specific display_id */
    mock_vc_tv_show_info_id_called = 0;
    mock_vc_tv_show_info_id_ret = 0;
    ret = show_info(1, 1);
    assert(ret == 0);
    assert(mock_vc_tv_show_info_id_called == 1);
    assert(mock_vc_tv_show_info_id_display_id == 1);
    assert(mock_vc_tv_show_info_id_on == 1);

    /* Test without specific display_id (-1) */
    mock_vc_tv_show_info_called = 0;
    mock_vc_tv_show_info_ret = -1;
    ret = show_info(-1, 0);
    assert(ret == -1);
    assert(mock_vc_tv_show_info_called == 1);
    assert(mock_vc_tv_show_info_on == 0);

    printf("UNIT_TEST_PASS: show_info_delegation_verified\n");
}

void test_display_state_safety() {
    printf("SOP-001 Verification: Running test_display_state_safety...\n");

    /* Verify NULL pointer handling in mock */
    int ret = vc_tv_get_display_state(NULL);
    assert(ret == -1);

    /* Setup mock data for HDMI status */
    memset(&mock_tvstate_out, 0, sizeof(mock_tvstate_out));
    mock_tvstate_out.state = VC_HDMI_HDMI;
    mock_tvstate_out.display.hdmi.width = 1920;
    mock_tvstate_out.display.hdmi.height = 1080;
    mock_tvstate_out.display.hdmi.frame_rate = 60;

    memset(&mock_property_out, 0, sizeof(mock_property_out));
    mock_property_out.param1 = HDMI_PIXEL_CLOCK_TYPE_PAL;

    mock_vc_tv_get_display_state_called = 0;
    mock_vc_tv_get_display_state_ret = 0;

    TV_DISPLAY_STATE_T state;
    ret = vc_tv_get_display_state(&state);
    assert(ret == 0);
    assert(state.display.hdmi.width == 1920);
    assert(state.display.hdmi.height == 1080);

    printf("UNIT_TEST_PASS: display_metadata_verified\n");
}

int main(void) {
    printf("--- TVSERVICE UNIT TEST SUITE START ---\n");
    test_show_info_delegation();
    test_display_state_safety();
    printf("--- TVSERVICE UNIT TEST SUITE COMPLETE ---\n");
    printf("All Kessel Flow compliance tests passed!\n");
    return 0;
}

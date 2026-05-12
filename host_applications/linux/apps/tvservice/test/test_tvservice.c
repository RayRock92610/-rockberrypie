#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "bcm_host.h"
#include "interface/vmcs_host/vc_tvservice.h"

/* Mock dependencies */
int vc_tv_hdmi_power_on_preferred_id_called = 0;
int vc_tv_hdmi_power_on_preferred_id_arg = 0;
int vc_tv_hdmi_power_on_preferred_id_ret = 0;

int vc_tv_hdmi_power_on_preferred_called = 0;
int vc_tv_hdmi_power_on_preferred_ret = 0;

int vc_tv_hdmi_power_on_preferred_id(uint32_t display_id) {
    vc_tv_hdmi_power_on_preferred_id_called = 1;
    vc_tv_hdmi_power_on_preferred_id_arg = display_id;
    return vc_tv_hdmi_power_on_preferred_id_ret;
}

int vc_tv_hdmi_power_on_preferred(void) {
    vc_tv_hdmi_power_on_preferred_called = 1;
    return vc_tv_hdmi_power_on_preferred_ret;
}

/* Intercept macros with varying args */
#define vcos_event_wait(...) 0
#define vc_tv_unregister_callback(...) 0
#define vcos_log_set_level(...) 0
#define bcm_host_init(...) 0
#define vcos_event_create(...) 0
#define vc_vchi_tv_init(...) 0
#define vc_tv_register_callback(...) 0
#define vchi_initialise(...) 0
#define vchi_connect(...) 0
#define vcos_event_signal(...) 0
#define vcos_event_destroy(...) 0
#define vc_vchi_tv_stop(...) 0
#define vc_tv_hdmi_get_supported_modes(...) 0
#define vc_tv_get_state(...) 0
#define vcos_pthreads_map_errno(...) 0
#define vcos_safe_vsprintf(...) 0
#define vcos_pthreads_logging_assert(...) 0
#define vcos_safe_sprintf(...) 0
#define bcm_host_is_kms_active(...) 0
#define vcos_init(...) 0
#define vchi_disconnect(...) 0

#define vc_tv_hdmi_get_supported_modes_new_id(...) 0
#define vc_tv_hdmi_get_supported_modes_new(...) 0
#define vc_tv_get_display_state_id(...) 0
#define vc_tv_get_display_state(...) 0
#define vc_tv_hdmi_get_property_id(...) 0
#define vc_tv_hdmi_get_property(...) 0
#define vc_tv_hdmi_audio_supported_id(...) 0
#define vc_tv_hdmi_audio_supported(...) 0
#define vc_tv_hdmi_ddc_read_id(...) 0
#define vc_tv_hdmi_ddc_read(...) 0
#define vc_tv_show_info_id(...) 0
#define vc_tv_show_info(...) 0
#define vc_tv_hdmi_power_on_explicit_id(...) 0
#define vc_tv_hdmi_power_on_explicit_new(...) 0
#define vc_tv_hdmi_set_property_id(...) 0
#define vc_tv_hdmi_set_property(...) 0
#define vc_tv_sdtv_power_on_id(...) 0
#define vc_tv_sdtv_power_on(...) 0
#define vc_tv_power_off_id(...) 0
#define vc_tv_power_off(...) 0
#define vc_tv_get_attached_devices(...) 0
#define vc_tv_get_device_id_id(...) 0
#define vc_tv_get_device_id(...) 0

#define main original_main
#define LOG_STD(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#include "../tvservice.c"

#undef main

void test_power_on_preferred_success(void) {
    vc_tv_hdmi_power_on_preferred_called = 0;
    vc_tv_hdmi_power_on_preferred_ret = 0;

    int result = power_on_preferred(-1);

    assert(vc_tv_hdmi_power_on_preferred_called == 1);
    assert(result == 0);
    printf("test_power_on_preferred_success passed\n");
}

void test_power_on_preferred_failure(void) {
    vc_tv_hdmi_power_on_preferred_called = 0;
    vc_tv_hdmi_power_on_preferred_ret = -1;

    int result = power_on_preferred(-1);

    assert(vc_tv_hdmi_power_on_preferred_called == 1);
    assert(result == -1);
    printf("test_power_on_preferred_failure passed\n");
}

void test_power_on_preferred_id_success(void) {
    vc_tv_hdmi_power_on_preferred_id_called = 0;
    vc_tv_hdmi_power_on_preferred_id_ret = 0;

    int result = power_on_preferred(2);

    assert(vc_tv_hdmi_power_on_preferred_id_called == 1);
    assert(vc_tv_hdmi_power_on_preferred_id_arg == 2);
    assert(result == 0);
    printf("test_power_on_preferred_id_success passed\n");
}

void test_power_on_preferred_id_failure(void) {
    vc_tv_hdmi_power_on_preferred_id_called = 0;
    vc_tv_hdmi_power_on_preferred_id_ret = -1;

    int result = power_on_preferred(3);

    assert(vc_tv_hdmi_power_on_preferred_id_called == 1);
    assert(vc_tv_hdmi_power_on_preferred_id_arg == 3);
    assert(result == -1);
    printf("test_power_on_preferred_id_failure passed\n");
}

int main() {
    test_power_on_preferred_success();
    test_power_on_preferred_failure();
    test_power_on_preferred_id_success();
    test_power_on_preferred_id_failure();
    return 0;
}

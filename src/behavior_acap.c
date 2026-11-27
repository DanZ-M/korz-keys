/*
 * korz-keys: &acap <letter>. Types the letter as a capital whatever the Caps Lock state of the
 * computer: with Caps Lock off it sends Shift + letter, with Caps Lock on it sends the plain
 * letter (Shift would turn it lowercase). Other keys are sent unchanged.
 * Based on ZMK's key press behavior (MIT, ZMK contributors).
 */
#define DT_DRV_COMPAT korz_behavior_acap

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/hid.h>
#include <zmk/hid_indicators.h>
#include <zmk/events/keycode_state_changed.h>
#include <dt-bindings/zmk/modifiers.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define CAPS_LOCK_BIT BIT(1) /* bit 1 of the host LED report, the same bit keyfx uses */

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata param_values[] = {
    {
        .display_name = "Key",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_HID_USAGE,
    },
};

static const struct behavior_parameter_metadata_set param_metadata_set[] = {{
    .param1_values = param_values,
    .param1_values_len = ARRAY_SIZE(param_values),
}};

static const struct behavior_parameter_metadata metadata = {
    .sets_len = ARRAY_SIZE(param_metadata_set),
    .sets = param_metadata_set,
};
#endif

static bool shifted; /* what the last press sent, so the release matches it */

static bool is_letter(uint32_t encoded) {
    uint16_t page = ZMK_HID_USAGE_PAGE(encoded);
    uint16_t id = ZMK_HID_USAGE_ID(encoded);

    return (page == 0 || page == HID_USAGE_KEY) && id >= HID_USAGE_KEY_KEYBOARD_A &&
           id <= HID_USAGE_KEY_KEYBOARD_Z;
}

static uint32_t encode(uint32_t param, bool shift) {
    return shift ? (param | (MOD_LSFT << 24)) : param;
}

static int on_acap_pressed(struct zmk_behavior_binding *binding,
                           struct zmk_behavior_binding_event event) {
    bool caps_lock = (zmk_hid_indicators_get_current_profile() & CAPS_LOCK_BIT) != 0;

    shifted = is_letter(binding->param1) && !caps_lock;
    LOG_DBG("acap 0x%02X caps lock %d shift %d", binding->param1, caps_lock, shifted);
    return raise_zmk_keycode_state_changed_from_encoded(encode(binding->param1, shifted), true,
                                                        event.timestamp);
}

static int on_acap_released(struct zmk_behavior_binding *binding,
                            struct zmk_behavior_binding_event event) {
    return raise_zmk_keycode_state_changed_from_encoded(encode(binding->param1, shifted), false,
                                                        event.timestamp);
}

static const struct behavior_driver_api behavior_acap_driver_api = {
    .binding_pressed = on_acap_pressed,
    .binding_released = on_acap_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &metadata,
#endif
};

#define ACAP_INST(n)                                                                               \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_acap_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ACAP_INST)

#endif

/*
 * korz-keys: back to the default layer after a quiet period.
 * Quiet = no key pressed, no layer change and no pointing input (joystick, air mouse) for
 * KORZ_LAYER_TIMEOUT_S seconds, and no key held down. Layers in KORZ_LAYER_TIMEOUT_KEEP_MASK
 * (bit = layer number) are left alone.
 */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#if IS_ENABLED(CONFIG_INPUT)
#include <zephyr/input/input.h>
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct k_work_delayable idle_work;
static int keys_down;

static void restart(void)
{
	k_work_reschedule(&idle_work, K_SECONDS(CONFIG_KORZ_LAYER_TIMEOUT_S));
}

static void idle_fn(struct k_work *work)
{
	if (keys_down > 0) { /* something is held: not idle */
		restart();
		return;
	}

	zmk_keymap_layers_state_t state = zmk_keymap_layer_state();

	for (int i = 0; i < ZMK_KEYMAP_LAYERS_LEN; i++) {
		if ((state & BIT(i)) && i != zmk_keymap_layer_default() &&
		    !(CONFIG_KORZ_LAYER_TIMEOUT_KEEP_MASK & BIT(i))) {
			LOG_INF("layer timeout: leaving layer %d", i);
			zmk_keymap_layer_deactivate(i, true); /* true: also clears a toggle */
		}
	}
}

static int on_position(const zmk_event_t *eh)
{
	const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);

	if (ev != NULL) {
		if (ev->state) {
			keys_down++;
		} else if (keys_down > 0) {
			keys_down--;
		}
	}
	restart();
	return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(korz_layer_timeout, on_position);
ZMK_SUBSCRIPTION(korz_layer_timeout, zmk_position_state_changed);
ZMK_SUBSCRIPTION(korz_layer_timeout, zmk_layer_state_changed);

#if IS_ENABLED(CONFIG_INPUT)
static int64_t last_input_ms;

static void on_input(struct input_event *evt, void *user_data)
{
	int64_t now = k_uptime_get();

	if (now - last_input_ms > 1000) { /* a pointer sends many events: look once a second */
		last_input_ms = now;
		restart();
	}
}
INPUT_CALLBACK_DEFINE(NULL, on_input, NULL);
#endif

static int layer_timeout_init(void)
{
	k_work_init_delayable(&idle_work, idle_fn);
	restart();
	return 0;
}
SYS_INIT(layer_timeout_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

# korz-keys

Small ZMK behaviors for the korz keyboard. One feature per release tag.

## korz hold-tap: a hold-tap inside a hold-tap

ZMK's hold-tap finds its active entry by key position only. A hold-tap placed in the tap
binding of another hold-tap on the same key (for example `tlt` -> tap dance -> `&LPT1`) then
gets the wrong entry on release, the inner release is lost and the hold stays pressed.
`korz,behavior-hold-tap` is a copy of ZMK's hold-tap (commit 4dd95d78, logic unchanged) with
its own list, so the two do not meet. Same properties as `zmk,behavior-hold-tap`. Use it only
for the inner hold-tap: set `compatible = "korz,behavior-hold-tap";` on that node.
Limits: `CONFIG_KORZ_HOLD_TAP_MAX_HELD` (10), `CONFIG_KORZ_HOLD_TAP_MAX_CAPTURED_EVENTS` (40).
After a ZMK update, compare `src/behavior_korz_hold_tap.c` with ZMK's `behavior_hold_tap.c`.

## v0.1.2: caps word with stop keys

v0.1: broke the peripheral half builds. v0.1.1: letters were not capitalised (the listener ran after
ZMK's HID listener). v0.1.2: the source is added to the app target so it runs before it.

A copy of ZMK's caps word (MIT, ZMK contributors) with two additions:

- `stop-list`: keys that end caps word even when they are modifiers (ZMK's own caps word
  ignores modifier keys). Example: `stop-list = <LCTRL>;` makes Left Ctrl end it.
- `keypad-numbers`: KP_N0..KP_N9 continue caps word like the number row does.

Use it by switching the compatible of the existing `&caps_word` node:

```
&caps_word {
	compatible = "korz,behavior-caps-word";
	continue-list = <UNDERSCORE MINUS DEL BACKSPACE DELETE>;
	stop-list = <LCTRL>;
	keypad-numbers;
};
```

The "active" flag stays the first member of the behavior's data, so keyfx's CI_CAPS_WORD
lights keep working. Remove the `compatible` line to go back to ZMK's own caps word.

## v0.2: &acap, a capital that ignores Caps Lock

With the computer's Caps Lock on, Shift + letter types a lowercase letter, so a macro made of
`&kp LS(V)` types `v`. `&acap V` reads the Caps Lock state the computer reports (host LED
report, needs `CONFIG_ZMK_HID_INDICATORS=y`) and sends Shift + V when it is off, plain V when
it is on: always a capital. Only letters are changed; any other key is sent as given.

```
#include <behaviors/acap.dtsi>      /* next to your other behavior includes */

VAL: VAL { compatible = "zmk,behavior-macro"; #binding-cells = <0>;
           bindings = <&macro_tap &acap V &acap A &acap L &acap V &kp KP_N0>; };
```

## v0.3: layer idle timeout

Back to the default layer (QWERTY) after a quiet period: no key, no layer change and no
pointing input (joystick, air mouse) for `CONFIG_KORZ_LAYER_TIMEOUT_S` seconds (default 120),
and no key held. Caps word ends too (`CONFIG_KORZ_LAYER_TIMEOUT_CAPS_WORD=n` to keep it). Off by default; in the dongle's .conf:

```
CONFIG_KORZ_LAYER_TIMEOUT=y
# CONFIG_KORZ_LAYER_TIMEOUT_S=120
# layers that never time out, bit = layer number (QWERTY 0, NUMB 1, MOUSE 2, FUNC 3, a design layer 4 = 0x10)
# CONFIG_KORZ_LAYER_TIMEOUT_KEEP_MASK=0x10
```

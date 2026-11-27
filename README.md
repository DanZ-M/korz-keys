# korz-keys

Small ZMK behaviors for the korz keyboard. One feature per release tag.

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

## Planned

v0.3 layer idle timeout: back to the default layer after a quiet period.

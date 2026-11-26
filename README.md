# korz-keys

Small ZMK behaviors for the korz keyboard. One feature per release tag.

## v0.1.1: caps word with stop keys (v0.1 broke the peripheral half builds)

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

## Planned

v0.2 `&acap`: a letter that is capital whatever the computer's Caps Lock state.
v0.3 layer idle timeout: back to the default layer after a quiet period.

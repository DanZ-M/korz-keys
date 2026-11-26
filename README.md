# korz-epaper

ZMK module: status screen and refresh policy for a small e-paper display (tested: WeAct 1.54" 200x200,
SSD1681, on the dongle of the korz keyboard). Everything that draws on the e-paper lives here, so the
screen can be changed or replaced without touching the keymap, the lights or the sensors.

| File | What it is |
|---|---|
| `src/epd_screen.c` | The status screen: output (USB / Bluetooth), profile, battery, the layers on a sphere with the volume arc (korz-media), the two keyboard halves. One block per widget; add a widget by copying one. |
| `src/epd_layout.h` | Where everything goes on the 200x200 panel: one number per position |
| `src/epd_icons.c/.h` | The pictures (generated, see below). |
| `src/epd_refresh.c/.h` | Refresh policy: partial updates stay local; a clean-up of just the changed area (black, white, redraw) runs after a pause or a number of changes. |
| `tools/gen_icons.py`, `tools/sphere.py` | Regenerate the pictures (Pillow + numpy). The sources of the old OLED icons are in `tools/legacy-icons`. |

## Use

1. West manifest: add this repo as a project (see the config repo's `config/west.yml`).
2. The wiring (SPI pins, `zephyr,display`, the `partial` refresh profile) belongs to your shield; the
   korz config repo has it in `boards/shields/korzdongle_addons/korzdongle_epaper.overlay`.
3. In the shield's `.conf`:
   ```
   CONFIG_SSD16XX=y
   CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM=y
   CONFIG_KORZ_EPD_SCREEN=y
   ```

## Settings (Kconfig)

| Symbol | Default | |
|---|---|---|
| `KORZ_EPD_REFRESH` | y | Clean-up policy on/off (needs the SSD1681 node) |
| `KORZ_EPD_FULL_IDLE_S` | 20 | Clean-up this many seconds after the last change |
| `KORZ_EPD_FULL_AFTER_CHANGES` | 12 | ... or after this many changes in a row (0 = only when idle) |
| `KORZ_EPD_CLEAN_LOCAL` | y | Clean only the changed area; n = whole panel flashes |
| `KORZ_EPD_SCREEN` | | The status screen above (needs `ZMK_DISPLAY_STATUS_SCREEN_CUSTOM`) |
| `KORZ_EPD_SIDES` | y (dongle) | Two small keyboards at the bottom: keys = half connected, dotted = not. Which half is which is learned from the first key pressed on it |
| `KORZ_EPD_SIDES_POLL_MS` | 500 | How often the connection is checked |

## Changing the pictures

`python3 tools/gen_icons.py` rewrites `src/epd_icons.c/.h` (pictures are 1-bit LVGL 9 images). The layer
sphere is drawn by `tools/sphere.py` (preview: `docs/layer-sphere.png`): row order, font, sizes and the
dither at the rim are constants at its top. The layer order is the one of the keymap: 0 BASE, 1 NUMB,
2 MOUSE, 3 FUNC.

## Notes

- Widgets only call `lv_image_set_src` when the picture really changes (a change is a panel refresh).
- Not here on purpose: anything about lights (keyfx), the motion sensor (korz-imu), the joystick (korz-joystick).
- The old OLED icons (battery, output, profile) come from HellTM's custom widgets (MIT).

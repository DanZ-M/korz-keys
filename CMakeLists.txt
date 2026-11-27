if(CONFIG_KORZ_CAPS_WORD)
  # Added to the app target, not to a module library: caps word must see a key event BEFORE
  # ZMK's HID listener sends it to the computer, and the order of listeners is the link order
  # of the object files. Module libraries are linked after the app, which is too late.
  target_sources(app PRIVATE src/behavior_korz_caps_word.c)
endif()

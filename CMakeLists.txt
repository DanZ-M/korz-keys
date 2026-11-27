# Added to the app target (not to a module library): for caps word the listener has to come
# before ZMK's HID listener, and that order is the link order of the object files.
if(CONFIG_KORZ_CAPS_WORD)
  target_sources(app PRIVATE src/behavior_korz_caps_word.c)
endif()
if(CONFIG_KORZ_ACAP)
  target_sources(app PRIVATE src/behavior_acap.c)
endif()

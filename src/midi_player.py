import common

raw_mode = False
usb_mode = False


def set_usb_mode(state):
    print("set_usb_mode %s" % (state))
    global usb_mode
    usb_mode = state


def set_raw_mode(state):
    print("set_raw_mode %s" % (state))
    global raw_mode
    raw_mode = state


def note_on(channel, midi_index, velocity):
    print("note_on", channel, midi_index, velocity)
    if raw_mode:
        common.send_raw_midi_message([channel + 0x90 - 1, midi_index, velocity])
    if usb_mode:
        common.send_usb_midi_message([channel + 0x90 - 1, midi_index, velocity])


def note_off(channel, midi_index):
    print("note_off", channel, midi_index)
    if raw_mode:
        common.send_raw_midi_message([channel + 0x80 - 1, midi_index, 0])
    if usb_mode:
        common.send_usb_midi_message([channel + 0x80 - 1, midi_index, 0])


def cc_message(channel, cc, value):
    print("cc", channel, cc, value)
    if raw_mode:
        common.send_raw_midi_message([channel + 0xB0 - 1, cc, value])
    if usb_mode:
        common.send_usb_midi_message([channel + 0xB0 - 1, cc, value])

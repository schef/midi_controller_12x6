import common

NOTE_ON_WITHOUT_CHANNEL = 0x90 - 1
NOTE_OFF_WITHOUT_CHANNEL = 0x80 - 1
CC_WITHOUT_CHANNEL = 0xB0 - 1
PROGRAM_CHANGE_WITHOUT_CHANNEL = 0xC0 - 1


def note_on(channel, midi_index, velocity):
    print("note_on", channel, midi_index, velocity)
    common.send_usb_midi_message([channel + NOTE_ON_WITHOUT_CHANNEL, midi_index, velocity])


def note_off(channel, midi_index):
    print("note_off", channel, midi_index)
    common.send_usb_midi_message([channel + NOTE_OFF_WITHOUT_CHANNEL, midi_index, 0])


def cc_message(channel, cc, value):
    print("cc", channel, cc, value)
    common.send_usb_midi_message([channel + CC_WITHOUT_CHANNEL, cc, value])


def program_change(channel, value):
    print("program change", channel, value)
    common.send_usb_midi_message([channel + PROGRAM_CHANGE_WITHOUT_CHANNEL, value])

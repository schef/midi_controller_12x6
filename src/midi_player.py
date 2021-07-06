import common

def note_on(channel, midi_index, velocity):
    print("note_on", channel, midi_index, velocity)
    common.send_usb_midi_message([channel + 0x90 - 1, midi_index, velocity])


def note_off(channel, midi_index):
    print("note_off", channel, midi_index)
    common.send_usb_midi_message([channel + 0x80 - 1, midi_index, 0])

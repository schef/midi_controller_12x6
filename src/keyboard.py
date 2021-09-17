import peripherals
import midi_player
import common

CHANNEL = 1


def get_button_midi_num(select_index, data_index):
    midi_index = -1
    # lowerKeys select_index[0-6], data_index[0-4], lowest pitch f-41
    if data_index < 5:
        midi_index = 41 + select_index * 5 + data_index * 1
    # lowerKeys select_index[0-6], data_index[7-11], lowest pitch f-65
    elif data_index > 6:
        midi_index = 65 - 7 + select_index * 5 + data_index * 1
    return midi_index


def get_pot_midi_num(pot_index, state):
    if pot_index == 0:
        return 7, int(state / 8)
    elif pot_index == 1:
        return 70, int(state / 4)
    return -1


def on_button_change(select_index, data_index, state):
    midi_index = get_button_midi_num(select_index, data_index)
    if midi_index != -1:
        if state:
            midi_player.note_on(CHANNEL, midi_index, 100)
        else:
            midi_player.note_off(CHANNEL, midi_index)
    else:
        if select_index == 5 and data_index == 5 and state:
            midi_player.set_usb_mode(True)
        if select_index == 5 and data_index == 6 and state:
            midi_player.set_raw_mode(True)


def on_pot_change(index, state):
    cc, new_state = get_pot_midi_num(index, state)
    if cc != -1:
        midi_player.cc_message(CHANNEL, cc, new_state)


def init():
    peripherals.register_on_button_change_cb(on_button_change)
    peripherals.register_on_pot_change_cb(on_pot_change)


def loop():
    pass

import peripherals
import midi_player
import common

CHANNEL = 1
HAMMOND_VOLUME_CC = 35
HAMMOND_EXPRESSION_CC = 2
HAMMOND_LESLIE_SPEED_CC = 1
HAMMOND_LESLIE_BREAK_CC = 93

program_change_index = 0


def get_button_midi_num(select_index, data_index):
    midi_index = -1
    # lowerKeys select_index[0-6], data_index[0-4], lowest pitch f-41
    if data_index < 5:
        midi_index = 41 + select_index * 5 + data_index * 1
    # lowerKeys select_index[0-6], data_index[7-11], lowest pitch f-65
    elif data_index > 6:
        midi_index = 65 - 7 + select_index * 5 + data_index * 1
    return midi_index


def on_button_change(select_index, data_index, state):
    global program_change_index
    midi_index = get_button_midi_num(select_index, data_index)
    if midi_index != -1:
        if state:
            midi_player.note_on(CHANNEL, midi_index, 100)
        else:
            midi_player.note_off(CHANNEL, midi_index)
    else:
        if state:
            if data_index == 5:
                if program_change_index > 0:
                    program_change_index -= 1
                midi_player.program_change(CHANNEL, program_change_index)
            elif data_index == 6:
                if program_change_index < 3:
                    program_change_index += 1
                midi_player.program_change(CHANNEL, program_change_index)


def on_pot_change(index, state):
    if index == 0:
        midi_player.cc_message(CHANNEL, HAMMOND_VOLUME_CC, state)
    elif index == 1:
        midi_player.cc_message(CHANNEL, HAMMOND_LESLIE_SPEED_CC, state)
        if state > 64 - 10 and state < 64 + 10:
            midi_player.cc_message(CHANNEL, HAMMOND_LESLIE_BREAK_CC, state)


def init():
    peripherals.register_on_button_change_cb(on_button_change)
    peripherals.register_on_pot_change_cb(on_pot_change)


def loop():
    pass

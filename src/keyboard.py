import peripherals
import midi_player


def get_button_midi_num(select_index, data_index):
    midi_index = -1
    # lowerKeys select_index[0-6], data_index[0-4], lowest pitch f-41
    if data_index < 5:
        midi_index = 41 + select_index * 5 + data_index * 1
    # lowerKeys select_index[0-6], data_index[7-11], lowest pitch f-65
    elif data_index > 6:
        midi_index = 65 - 7 + select_index * 5 + data_index * 1
    return midi_index

last_state = 0
def get_pot_midi_num(pot_index, state):
    global last_state
    if pot_index == 0:
        return 1, state
        #new_state = int(state/42)
        #if last_state != new_state:
        #    last_state = new_state
        #    return 91, new_state
        #else:
        #    return -1, 0
    elif pot_index == 1:
        return 11, state


def on_button_change(select_index, data_index, state):
    midi_index = get_button_midi_num(select_index, data_index)
    if midi_index != -1:
        if state:
            midi_player.note_on(1, midi_index, 100)
        else:
            midi_player.note_off(1, midi_index)


def on_pot_change(index, state):
    cc, new_state = get_pot_midi_num(index, state)
    if cc != -1:
        midi_player.cc_message(1, cc, new_state)


def init():
    peripherals.register_on_button_change_cb(on_button_change)
    peripherals.register_on_pot_change_cb(on_pot_change)


def loop():
    pass

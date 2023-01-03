import peripherals
import midi_player
import common

CHANNEL = 1
HAMMOND_VOLUME_CC = 7
HAMMOND_EXPRESSION_CC = 11
HAMMOND_LESLIE_SPEED_CC = 1
HAMMOND_LESLIE_BREAK_CC = 93

break_state = False

drawbar_preset_index = 0
DRAWBAR_PRESETS = [
    [8, 8, 0, 0, 0, 0, 0, 0, 0],
    [8, 8, 1, 1, 1, 0, 0, 0, 0],
    [8, 8, 2, 2, 2, 0, 0, 0, 0],
    [8, 8, 2, 2, 2, 2, 2, 0, 0],
    [8, 8, 4, 4, 2, 2, 2, 1, 1],
    [8, 8, 6, 6, 4, 4, 2, 2, 2],
    [8, 8, 8, 8, 6, 6, 4, 4, 4],
    [8, 8, 8, 8, 8, 8, 8, 8, 8],
]

DRAWBAR_MIDI_CC = [70, 71, 72, 73, 74, 75, 76, 77, 78]

bass_enabled = False


def set_drawbar_preset(index):
    for i in range(len(DRAWBAR_MIDI_CC)):
        value = 120 - DRAWBAR_PRESETS[index][i] * 16
        if value < 0:
            value = 0
        if value > 120:
            value = 120
        midi_player.cc_message(channel=CHANNEL, cc=DRAWBAR_MIDI_CC[i], value=value)


def get_button_midi_num(select_index, data_index):
    midi_index = -1
    # lowerKeys select_index[0-6], data_index[0-4], lowest pitch f-41
    if select_index == 5 and data_index == 0:
        pass
    elif data_index < 5:
        midi_index = 41 + select_index * 5 + data_index * 1
    # lowerKeys select_index[0-6], data_index[7-11], lowest pitch f-65
    elif data_index > 6:
        midi_index = 65 - 7 + select_index * 5 + data_index * 1
    return midi_index


def on_button_change(select_index, data_index, state):
    global drawbar_preset_index, bass_enabled
    midi_index = get_button_midi_num(select_index, data_index)
    if midi_index != -1:
        if state:
            midi_player.note_on(CHANNEL, midi_index, 100)
            if bass_enabled:
                midi_player.note_on(CHANNEL + 2, midi_index - 12, 100)
        else:
            midi_player.note_off(CHANNEL, midi_index)
            if bass_enabled:
                midi_player.note_off(CHANNEL + 2, midi_index - 12)
    else:
        if state:
            if select_index == 5 and data_index == 5:
                if drawbar_preset_index > 0:
                    drawbar_preset_index -= 1
                set_drawbar_preset(drawbar_preset_index)
            elif select_index == 5 and data_index == 6:
                if drawbar_preset_index < len(DRAWBAR_PRESETS) - 1:
                    drawbar_preset_index += 1
                set_drawbar_preset(drawbar_preset_index)
            elif select_index == 5 and data_index == 0:
                if bass_enabled:
                    bass_enabled = False
                else:
                    bass_enabled = True
                print("bass_enabled", str(bass_enabled))


def on_pot_change(index, state):
    global break_state
    if index == 0:
        midi_player.cc_message(CHANNEL, HAMMOND_VOLUME_CC, state)
    elif index == 1:
        midi_player.cc_message(CHANNEL, HAMMOND_LESLIE_SPEED_CC, state)
        if state > 30 and state < 90:
            if not break_state:
                break_state = True
                midi_player.cc_message(CHANNEL, HAMMOND_LESLIE_BREAK_CC, 127)
        else:
            if break_state:
                break_state = False
                midi_player.cc_message(CHANNEL, HAMMOND_LESLIE_BREAK_CC, 0)


def init():
    peripherals.register_on_button_change_cb(on_button_change)
    peripherals.register_on_pot_change_cb(on_pot_change)


def loop():
    pass

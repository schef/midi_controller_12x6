import peripherals
import midi_player
import common

FIRST_CHANNEL = 1
SECOND_CHANNEL = 2
BASS_CHANNEL = 3
HAMMOND_VOLUME_CC = 7
HAMMOND_EXPRESSION_CC = 11
HAMMOND_LESLIE_SPEED_CC = 1
POT_STATE_CC = 64

drawbar_preset_index = 0
UPPER_DRAWBAR_PRESETS = [
    [8, 0, 0, 0, 0, 0, 0, 0, 0],
    [8, 8, 0, 0, 0, 0, 0, 0, 0],
    [8, 8, 6, 0, 0, 0, 0, 0, 0],
    [8, 8, 6, 2, 2, 0, 0, 0, 0],
    [8, 8, 6, 4, 4, 2, 2, 0, 0],
    [8, 8, 6, 6, 5, 4, 2, 0, 0],
    [8, 8, 6, 3, 3, 2, 2, 1, 1],
    [8, 8, 6, 3, 3, 3, 3, 2, 2],
]

LOWER_DRAWBAR_PRESETS = [
    [8, 3, 8, 0, 0, 0, 0, 0, 0],
]

PEDAL_DRAWBAR_PRESETS = [
    [8, 6, 0, 0, 0, 0, 0, 0, 0],
]

DRAWBAR_MIDI_CC = [70, 71, 72, 73, 74, 75, 76, 77, 78]

fn_key_pressed = False
bass_enabled = False


def set_drawbar(index, value, channel):
    value = 120 - value * 16
    if value < 0:
        value = 0
    if value > 120:
        value = 120
    midi_player.cc_message(channel=channel, cc=DRAWBAR_MIDI_CC[index], value=value)


def set_drawbar_preset(preset, channel):
    for i in range(len(preset)):
        value = preset[i]
        set_drawbar(i, value, channel)

def set_vibrato(state):
    midi_player.cc_message(channel=FIRST_CHANNEL, cc=95, value=127 if state else 0)

def set_percussion(state):
    midi_player.cc_message(channel=FIRST_CHANNEL, cc=80, value=127 if state else 0)

def set_soft_percussion(state):
    midi_player.cc_message(channel=FIRST_CHANNEL, cc=81, value=127 if state else 0)

def set_vibrato_routing(num):
    midi_player.cc_message(channel=FIRST_CHANNEL, cc=92, value=num)

# s:d #############################################
# 5:0 5:1 5:2 5:3 5:4 5:5 5:6 5:7 5:7 5:9 5:15 5:11
# 4:0 4:1 4:2 4:3 4:4 _:_ _:_ 4:7 4:7 4:9 4:14 4:11
# 3:0 3:1 3:2 3:3 3:4 _:_ _:_ 3:7 3:7 3:9 3:13 3:11
# 2:0 2:1 2:2 2:3 2:4 _:_ _:_ 2:7 2:7 2:9 2:12 2:11
# 1:0 1:1 1:2 1:3 1:4 _:_ _:_ 1:7 1:7 1:9 1:11 1:11
# 0:0 0:1 0:2 0:3 0:4 _:_ _:_ 0:7 0:7 0:9 0:10 0:11

def get_button_midi_num_and_channel(select_index, data_index):
    midi_index = -1
    channel = -1
    # FN_KEY
    if select_index == 5 and data_index == 0:
        pass
    # lowKeys select_index[0-6], data_index[0-4], lowest pitch f-41
    elif data_index < 5 and select_index < 5:
        midi_index = 41 + select_index * 5 + data_index * 1 #
        channel = SECOND_CHANNEL
    # highKeys select_index[0-6], data_index[7-11], lowest pitch f-65
    elif data_index > 6:
        midi_index = 65 - 7 + select_index * 5 + data_index * 1
        channel = FIRST_CHANNEL
    return midi_index, channel


def on_button_change(select_index, data_index, state):
    global drawbar_preset_index, bass_enabled, fn_key_pressed
    if select_index == 5 and data_index == 0:
        fn_key_pressed = state
    if not fn_key_pressed:
        midi_index, channel = get_button_midi_num_and_channel(select_index, data_index)
        if midi_index != -1:
            if state:
                midi_player.note_on(channel, midi_index, 100)
                if bass_enabled and midi_index < 65:
                    midi_player.note_on(BASS_CHANNEL, midi_index - 12, 100)
            else:
                midi_player.note_off(channel, midi_index)
                if bass_enabled and midi_index < 65:
                    midi_player.note_off(BASS_CHANNEL, midi_index - 12)
        else:
            if state:
                # drawbar preset decrease
                if select_index == 5 and data_index == 5:
                    if drawbar_preset_index > 0:
                        drawbar_preset_index -= 1
                    set_drawbar_preset(UPPER_DRAWBAR_PRESETS[drawbar_preset_index], FIRST_CHANNEL)
                # drawbar preset increase
                elif select_index == 5 and data_index == 6:
                    if drawbar_preset_index < len(UPPER_DRAWBAR_PRESETS) - 1:
                        drawbar_preset_index += 1
                    set_drawbar_preset(UPPER_DRAWBAR_PRESETS[drawbar_preset_index], FIRST_CHANNEL)
                elif select_index == 5 and data_index == 1:
                    set_vibrato(False)
                elif select_index == 5 and data_index == 2:
                    set_vibrato(True)
                elif select_index == 5 and data_index == 3:
                    set_percussion(False)
                elif select_index == 5 and data_index == 4:
                    set_percussion(True)
    else:
        # turn off playing notes
        midi_index, channel = get_button_midi_num_and_channel(select_index, data_index)
        if midi_index != -1:
            if not state:
                midi_player.note_off(channel, midi_index)
                if bass_enabled and midi_index < 65:
                    midi_player.note_off(channel + 2, midi_index - 12)
        # bass handler
        if select_index == 4 and data_index == 0:
            bass_enabled = True
        elif select_index == 3 and data_index == 0:
            bass_enabled = False
        elif select_index == 5 and data_index == 5:
            set_drawbar_preset(LOWER_DRAWBAR_PRESETS[0], SECOND_CHANNEL)
            set_drawbar_preset(PEDAL_DRAWBAR_PRESETS[0], BASS_CHANNEL)
            set_soft_percussion(True)
            set_vibrato_routing(127)
        # drawbar manual
        elif data_index in [1, 2, 3, 4]:
            index = data_index - 1
            value = 5 - select_index
            set_drawbar(index, value)
        elif data_index in [7, 8, 9, 10, 11]:
            index = data_index - 3
            value = 5 - select_index
            set_drawbar(index, value)


def on_pot_change(index, state):
    global break_state
    if index == 0:
        midi_player.cc_message(FIRST_CHANNEL, HAMMOND_VOLUME_CC, state)
    elif index == 1:
        if state < 42:
            value = 0
        elif state > 84:
            value = 127
        else:
            value = 64
        midi_player.cc_message(FIRST_CHANNEL, HAMMOND_LESLIE_SPEED_CC, value)
        midi_player.cc_message(FIRST_CHANNEL, POT_STATE_CC, state)


def init():
    peripherals.register_on_button_change_cb(on_button_change)
    peripherals.register_on_pot_change_cb(on_pot_change)


def loop():
    pass

import peripherals
import midi_player

midi_notes_request = [False for i in range(128)]
midi_notes_field = [False for i in range(128)]

def get_button_midi_num(select_index, data_index):
    midi_index = -1
    #lowerKeys select_index[0-6], data_index[0-4], lowest pitch f-41
    if data_index < 5:
        midi_index = 41 + select_index * 5 + data_index * 1
    #lowerKeys select_index[0-6], data_index[7-11], lowest pitch f-65
    elif data_index > 6:
        midi_index = 65 - 7 + select_index * 5 + data_index * 1
    return midi_index

def get_pot_midi_num(pot_index):
    return 0

def init():
    pass

def loop():
    global midi_notes_request, midi_notes_field
    for key, state in peripherals.buttons_state.items():
        midi_index = get_button_midi_num(key[0], key[1])
        if midi_index != -1:
            midi_notes_request[midi_index] = state
            if state:
                print("midi index test", midi_index)
                
    for i in range(len(midi_notes_field)):
        if i % 8 == 7:
            print("[%d:%d]" % (midi_notes_request[i], midi_notes_field[i]))
        else:
            print("[%d:%d]" % (midi_notes_request[i], midi_notes_field[i]), end=" ")
    for midi_index in range(len(midi_notes_request)):
        if midi_notes_request[midi_index] != midi_notes_field[midi_index]:
            midi_notes_field[midi_index] = midi_notes_request[midi_index]
            print("midi_notes_field[%d] updated to %s" % (midi_index, midi_notes_field[midi_index]))
            #if midi_notes_request[midi_index]:
                #midi_player.note_on(1, midi_index, 100)
            #else:
                #midi_player.note_off(1, midi_index)
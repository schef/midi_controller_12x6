import common

buttons_select = []
buttons_data = []
buttons_state = {}
pots = []
pots_state = {}

on_button_change_cb = None
on_pot_change_cb = None


class Pot:
    def __init__(self):
        self.state = -1

    def get_state(self):
        return self.state

    def set_state(self, state):
        self.state = int(state)

    def is_new(self, state):
        state = int(state)
        return state != self.state and abs(abs(state) - abs(self.state)) > 2


def init():
    global buttons_select, buttons_data, buttons_state, pots, pots_state
    for pin in common.BUTTON_SELECT:
        buttons_select.append(common.create_output(pin))
        buttons_select[-1].value = False
    for pin in common.BUTTON_DATA:
        buttons_data.append(common.create_input(pin))
    for select_index, select in enumerate(buttons_select):
        for data_index, data in enumerate(buttons_data):
            buttons_state[(select_index, data_index)] = -1

    for pin in common.POTS:
        pots.append(common.create_analog_input(pin))
    for pot_index, pot in enumerate(pots):
        pots_state[pot_index] = Pot()


def loop():
    global buttons_select, buttons_data, buttons_state, pots, pots_state
    global on_button_change_cb, on_pot_change_cb
    for select_index, select in enumerate(buttons_select):
        select.value = True
        for data_index, data in enumerate(buttons_data):
            state = data.value
            if state != buttons_state[(select_index, data_index)]:
                buttons_state[(select_index, data_index)] = state
                print("button changed[%d:%d] = %d" %
                      (select_index, data_index, state))
                if on_button_change_cb:
                    on_button_change_cb(select_index, data_index, state)
        select.value = False

    for pot_index, pot in enumerate(pots):
        p = pots_state[pot_index]
        state = common.remap_pot(pot.value)
        if p.is_new(state):
            p.set_state(state)
            print("pot changed[%d] = %d" % (pot_index, p.get_state()))
            if on_pot_change_cb:
                on_pot_change_cb(pot_index, p.get_state())


def register_on_button_change_cb(cb):
    global on_button_change_cb
    on_button_change_cb = cb


def register_on_pot_change_cb(cb):
    global on_pot_change_cb
    on_pot_change_cb = cb


def test_loop():
    init()
    while True:
        loop()

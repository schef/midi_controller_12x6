from machine import Pin, ADC
from utime import sleep
from time import ticks_ms

LED_PIN = 25
BUTTON_SELECT = [16, 17, 18, 19, 20, 21]
BUTTON_DATA = [15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4]
POT_MEASURE_COUNTS = 100
POTS = [0, 1]


def get_millis():
    return ticks_ms()


def millis_passed(timestamp):
    return get_millis() - timestamp


def create_output(pin):
    return Pin(pin, Pin.OUT)


def create_input(pin):
    return Pin(pin, Pin.IN, Pin.PULL_DOWN)


def create_analog_input(pin):
    return ADC(pin)


def get_pot_mean(pot, counts):
    measures = [pot.read_u16() for i in range(counts)]
    return int(sum(measures)/counts)


def remap(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


def remap_pot(x):
    return remap(x, 0, 65535, 0, 127)


def test_peripherals():
    buttons_select = []
    buttons_data = []
    buttons_state = {}
    for pin in BUTTON_SELECT:
        buttons_select.append(create_output(pin))
        buttons_select[-1].off()
    for pin in BUTTON_DATA:
        buttons_data.append(create_input(pin))
    for select_index, select in enumerate(buttons_select):
        for data_index, data in enumerate(buttons_data):
            buttons_state[(select_index, data_index)] = -1

    pots = []
    pots_state = {}
    for pin in POTS:
        pots.append(create_analog_input(pin))
    for pot_index, pot in enumerate(pots):
        pots_state[pot_index] = -1

    while True:
        for select_index, select in enumerate(buttons_select):
            select.on()
            for data_index, data in enumerate(buttons_data):
                state = data.value()
                if state != buttons_state[(select_index, data_index)]:
                    print("button changed[%d:%d] = %d" %
                          (select_index, data_index, state))
                    buttons_state[(select_index, data_index)] = state
            select.off()

        for pot_index, pot in enumerate(pots):
            state = remap_pot(get_pot_mean(pot, POT_MEASURE_COUNTS))
            if abs(state - pots_state[pot_index]) >= 1:
                pots_state[pot_index] = state
                print("pot changed[%d] = %d" %
                      (pot_index, pots_state[pot_index]))

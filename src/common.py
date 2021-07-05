from machine import Pin, ADC
from utime import sleep
from time import ticks_ms

LED_PIN = 25
BUTTON_SELECT = [16, 17, 18, 19, 20, 21]
BUTTON_DATA = [15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4]
ANALOG_LEFT_PIN = 0
ANALOG_RIGHT_PIN = 0

A_LEFT_MIN = 10674
A_LEFT_MAX = 53212

A_RIGHT_MIN = 176
A_RIGHT_MAX = 1280


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


def test_peripherals():
    select = []
    data = []
    saved_state = {}
    for pin in BUTTON_SELECT:
        select.append(create_output(pin))
    for pin in BUTTON_DATA:
        data.append(create_input(pin))
    for s in select:
        s.off()
    for x, s in enumerate(select):
        for y, d in enumerate(data):
            saved_state[(x, y)] = 0
    pot_left = create_analog_input(0)
    pot_right = create_analog_input(2)
    pot_left_saved_state = pot_left.read_u16()
    pot_right_saved_state = pot_right.read_u16()
    pot_left_min = pot_left_saved_state
    pot_left_max = pot_left_saved_state
    pot_right_min = pot_right_saved_state
    pot_right_max = pot_right_saved_state
    print("pot changed[L] = %d <- %d -< %d" % (pot_left_min, pot_left_saved_state, pot_left_max))
    print("pot changed[R] = %d <- %d -< %d" % (pot_right_min, pot_right_saved_state, pot_right_max))

    while True:
        for x, s in enumerate(select):
            s.on()
            for y, d in enumerate(data):
                state = d.value()
                if state != saved_state[(x, y)]:
                    print("state changed[%d:%d] = %d" % (x, y, state))
                    saved_state[(x,y)] = state
            s.off()

        pot_left_state = pot_left.read_u16()
        if abs (pot_left_state - pot_left_saved_state) >= 10:
            pot_left_saved_state = pot_left_state
            if pot_left_saved_state < pot_left_min:
                pot_left_min = pot_left_saved_state
            if pot_left_saved_state > pot_left_max:
                pot_left_max = pot_left_saved_state
            print("pot changed[L] = %d <- %d -< %d" % (pot_left_min, pot_left_saved_state, pot_left_max))
        #1/5  144 <- 800 -< 1104
        #2/5
        #3/5 1824 <- 1840 -< 1920
        #4/5
        #5/5
        #min max 

        pot_right_state = pot_right.read_u16()
        if abs(pot_right_state - pot_right_saved_state) >= 10:
            pot_right_saved_state = pot_right_state
            if pot_right_saved_state < pot_right_min:
                pot_right_min = pot_right_saved_state
            if pot_right_saved_state > pot_right_max:
                pot_right_max = pot_right_saved_state
            print("pot changed[R] = %d <- %d -< %d" % (pot_right_min, pot_right_saved_state, pot_right_max))
        #1/5 0 <- 384 -< 544
        #2/5
        #3/5 2064 <- 2080 -< 2144
        #4/5
        #5/5
        #min max 
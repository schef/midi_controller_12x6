import peripherals
import keyboard
import time

def init():
    peripherals.init()
    keyboard.init()


def loop():
    while True:
        peripherals.loop()
        keyboard.loop()
        time.sleep(0.01)


def test():
    init()
    loop()

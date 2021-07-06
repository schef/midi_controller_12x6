import common
import peripherals
import keyboard
import time

def run():
    peripherals.init()
    keyboard.init()
    while True:
        peripherals.loop()
        keyboard.loop()
        time.sleep(1)

if __name__ == "__main__":
    run()

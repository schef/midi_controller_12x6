import peripherals
import keyboard

def init():
    peripherals.init()
    keyboard.init()

def loop():
    while True:
        peripherals.loop()
        keyboard.loop()
        
def test():
    init()
    loop()
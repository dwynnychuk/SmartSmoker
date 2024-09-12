# Functions for gui.py

# LED Functions
def turn_led_on():
    print("on")

def turn_led_off():
    print("off")

def blink_led():
    pass

# Connection Functions
def connect_to_grill():
    pass

def disconnect_from_grill():
    pass

# Measurement Functions


# Logging Functions
def logging_start():
    pass

def logging_stop():
    pass

# Serial Functions
def read_serial():
    pass

def print_hello_world(window, end_marker, i=0):
    if i < 100:
        window.insert(end_marker, f"Hello world {i}\n")
        window.see(end_marker)
        window.after(100, print_hello_world, window, end_marker, i+1)

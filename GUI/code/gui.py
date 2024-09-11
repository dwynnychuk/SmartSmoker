from tkinter import *
from gui_functions import *
import serial
import time

# Initialize Tkinter window
root = Tk()
root.title("GUI Test")
root.geometry("300x200")

# Tkinter Widgets
#button_on = Button(root, text="Turn LED On", command=turn_led_on)
#button_on.pack(pady=10)

#button_off = Button(root, text="Turn LED Off", command=turn_led_off)
#button_off.pack(pady=10)

# Run the Tkinter event loop
root.mainloop()
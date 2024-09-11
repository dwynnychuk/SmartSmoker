from tkinter import *
from gui_functions import *
import serial
import time

# Initialize Tkinter window
root = Tk()
root.title("GUI Test")
root.geometry("300x200")

# Tkinter Widgets
# Connection Frame
connection_frame = LabelFrame(root, text="Connect to Grill")
connection_frame.grid(row=0, column=1, columnspan=3, padx=10, pady=10)

button_connect = Button(connection_frame, text="Connect", command=connect_to_grill)
button_connect.grid(row=0, column=0, padx=10, pady=10)

button_disconnect = Button(connection_frame, text="Disconnect", command=disconnect_from_grill)
button_disconnect.grid(row=0, column=1, padx=10, pady=10)

# Control Frame


# Settings Frame


# Output Frame


# Run the Tkinter event loop
root.mainloop()
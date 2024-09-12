from tkinter import *
from tkinter.scrolledtext import ScrolledText
import tkinter.ttk as ttk
from gui_functions import *
import serial
import time

# Variable Initializations
paddingX = 10
paddingY = 10

# Initialize Tkinter window
root = Tk()
root.title("GUI Test")
root.geometry("1000x800")

# Examples, need to connect
connectedPort = "COM4"
loggingFilepath = "C:/Desktop"

# Tkinter Widgets
# Connection Frame
connection_frame = LabelFrame(root, text="Connect to Grill")
connection_frame.grid(row=0, column=0, padx=paddingX, pady=paddingY, sticky=NSEW)

button_connect = Button(connection_frame, text="Connect", command=connect_to_grill)
button_connect.grid(row=0, column=0, padx=paddingX, pady=paddingY)

button_disconnect = Button(connection_frame, text="Disconnect", command=disconnect_from_grill)
button_disconnect.grid(row=0, column=1, padx=paddingX, pady=paddingY)

combobox_ports = ttk.Combobox(connection_frame)
combobox_ports.grid(row=1, column=0)

label_connectedto = Label(connection_frame, text=f"Connected To:  {connectedPort}")
label_connectedto.grid(row=2, column=0, columnspan=2, padx=paddingX, pady=paddingY)

# Control Frame
control_frame = LabelFrame(root, text="Grill Control")
control_frame.grid(row=1, column=0, padx=paddingX, pady= paddingY, sticky=NSEW)

button_startlog = Button(control_frame, text="Start Logging", command=logging_start)
button_startlog.grid(row=0, column=0, padx=paddingX, pady=paddingY)

button_stoplog = Button(control_frame, text="Stop Logging", command=logging_stop)
button_stoplog.grid(row=0, column=1, padx=paddingX, pady=paddingY)

label_loggingto = Label(control_frame, text=f"Logging Location: {loggingFilepath}")
label_loggingto.grid(row=1, column=0, padx=paddingX, pady=paddingY)

# Settings Frame

# Output Frame
window_output = ScrolledText(root, wrap=WORD, width=100, height=25)
window_output.grid(row=3, column=0, columnspan=4, padx=paddingX, pady=paddingY)
        
print_hello_world(window_output, END)

# Run the Tkinter event loop
root.mainloop()
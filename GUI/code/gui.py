from tkinter import *
from tkinter.scrolledtext import ScrolledText
import tkinter.ttk as ttk
from tkinter.filedialog import asksaveasfilename

import serial.tools
import serial.tools.list_ports
from gui_functions import *
import serial
import time
import datetime

# -------------------------------------------------------------------------- #
# Variable Initializations
# -------------------------------------------------------------------------- #
global updateTimeMs
global connectedPort
global ser
global loggingActive

paddingX = 10
paddingY = 10
updateTimeMs = 2000
loggingFilepath = None
logFile = None
connectedPort = None
ser = None
loggingActive = False

# Initialize Tkinter window
root = Tk()
root.title("GUI Test")
root.geometry("1000x800")

# -------------------------------------------------------------------------- #
# Functions
# -------------------------------------------------------------------------- #
def logging_start():
    global loggingFilepath, logFile, loggingActive
    tempLogFile = "log_" + datetime.datetime.now().strftime("%m-%d-%Y-%H-%M-%S")
    loggingFilepath = asksaveasfilename(defaultextension=".csv",
                                        initialdir="/Users/dallynwynnychuk/Documents/Projects/Smart Smoker/SmartSmoker/Data",
                                        initialfile=tempLogFile,
                                        filetypes=[("CSV files", "*.csv"),("All Files", "*.*")]) # Label pattern tuples
    if loggingFilepath:
        label_loggingto.config(text=f"Logging Location: {loggingFilepath}")
        button_startlog.config(state="disabled")
        loggingActive = True
        logFile = open(loggingFilepath,'w')
        logFile.write("Teme, Temp1, Light, LidAccX, LidAccY, LidAccZ, LidGyX, LidGyY, LidGyZ, \
                        HopAccX, HopAccY, HopAccZ, HopGyX, HopGyY, HopGyZ\n")
        read_serial()

def logging_stop():
    global logFile, loggingActive
    loggingActive = False
    if logFile:
        logFile.close()
        logFile = None
        button_startlog.config(state="normal")
        window_output.insert(END, "Logging stopped\n")
        window_output.see(END)

def blink_led():
    pass

def list_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]

def connect_to_grill():
    global ser, connectedPort
    selectedPort = combobox_ports.get()
    
    if selectedPort:
        try:
            ser = serial.Serial(selectedPort, baudrate=115200, timeout=1)
            connectedPort = selectedPort
            label_connectedto.config(text=f"Connected to {connectedPort}")
            window_output.insert(END, f"Connected to {connectedPort}\n")
        except serial.SerialException as e:
            window_output.insert(END, f"Failed to connect to {selectedPort}: {str(e)}\n")
    else:
        window_output.insert(END, "No port selected\n")
    window_output.see(END)
            
def disconnect_from_grill():
    global ser, connectedPort
    if ser and ser.is_open:
        ser.close()
        label_connectedto.config(text="Disconnected")
        window_output.insert(END, f"Disconnected from Device\n")
        window_output.see(END)
        ser = None
        connectedPort = None
    else:
        window_output.insert(END, "No active connection to disconnect\n")
    window_output.see(END)

def read_serial():
    global ser, logFile, loggingActive
    
    if not loggingActive:
        return
    
    if ser and ser.is_open:
        try:
            if ser.in_waiting > 0:
                line = ser.readline().decode("utf-8").strip()
                if line:
                    timeStamp = datetime.datetime.now().strftime("%H-%M-%S")
                    dataRaw = line.split(",")
                    # Current Layout Below:
                    # Ambient, Light Sensor, AccLx, AccLy, AccLz, GyLx, GyLy, GyLz, AccHx, AccHy, AccHz, GyHx, GyHy, GyHz
                    window_output.insert(END, f"{timeStamp}, {dataRaw[0]}, {dataRaw[1]}, {dataRaw[2]}, {dataRaw[3]}, {dataRaw[4]}, {dataRaw[5]}, {dataRaw[6]}, {dataRaw[7]}, {dataRaw[8]}, {dataRaw[9]}, {dataRaw[10]}, {dataRaw[11]}, {dataRaw[12]}, {dataRaw[13]}\n")
                    window_output.see(END)
                    
                    if logFile:
                        logFile.write(f"{timeStamp}, {dataRaw[0]}, {dataRaw[1]}, \
                                        {dataRaw[2]}, {dataRaw[3]}, {dataRaw[4]}, \
                                        {dataRaw[5]}, {dataRaw[6]}, {dataRaw[7]}, \
                                        {dataRaw[8]}, {dataRaw[9]}, {dataRaw[10]}, \
                                        {dataRaw[11]}, {dataRaw[12]}, {dataRaw[13]}\n")
                        logFile.flush()
        
        except serial.SerialException as e:
            window_output.insert(END, f"Error reading serial port, error: {str(e)}\n")
            window_output.see(END)
    else:
        window_output.insert(END, "No active serial connection.\n")
        window_output.see(END)
    
    if loggingActive:
        root.after(updateTimeMs, read_serial)
    

# -------------------------------------------------------------------------- #
# Tkinter Widgets
# -------------------------------------------------------------------------- #
# Connection Frame
connection_frame = LabelFrame(root, text="Connect to Grill")
connection_frame.grid(row=0, column=0, padx=paddingX, pady=paddingY, sticky=NSEW)

button_connect = Button(connection_frame, text="Connect", command=connect_to_grill)
button_connect.grid(row=0, column=0, padx=paddingX, pady=paddingY)

button_disconnect = Button(connection_frame, text="Disconnect", command=disconnect_from_grill)
button_disconnect.grid(row=0, column=1, padx=paddingX, pady=paddingY)

availablePorts = list_ports()
combobox_ports = ttk.Combobox(connection_frame, values=availablePorts)
combobox_ports.grid(row=1, column=0)
if availablePorts:
    combobox_ports.current(0)

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

# Info Frame
# Cover on or Off
# Lid open or closed


# Output Frame
window_output = ScrolledText(root, wrap=WORD, width=100, height=25)
window_output.grid(row=3, column=0, columnspan=4, padx=paddingX, pady=paddingY)

# -------------------------------------------------------------------------- #
# Run the Tkinter event loop
# -------------------------------------------------------------------------- #
root.mainloop()
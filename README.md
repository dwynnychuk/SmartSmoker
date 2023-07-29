# Smart-Smoker
Upgrade to my existing Traeger smoker BBQ to add intelligent functionality. Initial upgrades target data acquisition to monitor internal and ambient temperatures. The second stage of this project will use integrated sensor data to drive the smoker fan, auger, and hot end with a custom PID-based controller. 

# How it works
Firmware and iOS application in development. The devices will tie into the wall power on the control panel inside the smoker. An iOS device will be used to connect to the system once running, which will receive updates over BLE regarding the status of the cook. 

# Hardware
Main PCB  
- Custom implementation of ESP32-WROOM module for WiFi and BLE connectivity
- AC/DC rectifier with optional USB power handled with power path selector IC (LTC4412)
- Input for 4x K-Type thermocouples to monitor food temperature (x2), external ambient temperature, and internal ambient temperature. Analog multiplexer is used to allow the use of a single amplifier.
- Interface to connect to Lid and Hopper PCBs

Lid PCB  
- Ambient light sensor to detect if the user left the cover off after a cook
- IMU to be used as a forward feed input for the PID controller

Hopper PCB  
- Ambient light sensor to detect if the user left the cover off after a cook
- IMU to alert the user if the hopper lid is left open
- ToF sensor to read remaining pellet level

# Pinout
1 - I2C SDA  
2 - GND  
3 - I2C SCL  
4 - +3.3V  

# Image
<img width="987" alt="SmokerCad" src="https://github.com/dwynnychuk/SmartSmoker/assets/92692830/30f151fa-3eb1-4e98-861e-fa84921604d5">

# Suggested Changes for Next Hardware REV
- Delete 1x ambient light sensors
- Add a dedicated GND pin for easier use of a logic analyzer
- Breakout XSHUT and INT pins on main board for easier connection to hopper
- Footprint change on U4
- Add control for Traeger interface to implement PID control

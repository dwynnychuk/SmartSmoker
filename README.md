# Smart-Smoker
Upgrade to my existing Traeger smoker BBQ to add intelligent functionality. Initial upgrades target data acquisition to monitor internal and ambient temperatures. The second stage of this project will use integrated sensor data to drive the smoker fan, auger, and hot end with a custom PID-based controller. 

# How it works
Firmware and iOS application in development. 

# Hardware
Main PCB<br />
- Custom implementation of ESP32-WROOM module for WiFi and BLE connectivity
- AC/DC rectifier with optional USB power handled with power path selector IC (LTC4412)
- Input for 4x K-Type thermocouples to monitor food temperature (x2), external ambient temperature, and internal ambient temperature. Analog multiplexer is used to allow the use of a single amplifier.
- Interface to connect to LID and HOPPER PCBs


# Pinout
1 - I2C SDA<br/ >
2 - GND<br/ >
3 - I2C SCL<br/ >
4 - +3.3V<br/ >

# Image

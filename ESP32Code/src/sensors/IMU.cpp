#include "IMU.h"

IMU::IMU(*TwoWire wire, uint8_t, addr)
    : imu (wire, addr) {}
#include "IMU.h"

IMU::IMU(TwoWire* wire, uint8_t addr)
    : imu (wire, addr) {}

void IMU::begin() {
    imu.begin();
    imu.Enable_X(); // accelerometer
    imu.Enable_G(); // gyroscope
}

void IMU::update() {
    imu.Get_X_Axes(acc);
    imu.Get_G_Axes(gyro);
}

int32_t* IMU::getAcc() {
    return acc;
}

int32_t* IMU::getGyro() {
    return gyro;
}
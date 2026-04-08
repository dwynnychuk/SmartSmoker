#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include "ISensor.h"
#include <LSM6DSRSensor.h>

class IMU : public Isensor {
private:
    LSM6DSRSensor imu;
    int32_t acc[3];     // x,y,z acceleration
    int32_t gyro[3];    // x,y,z angular acceleration

public:
    IMU(TwoWire* wire, uint8_t addr);

    void begin() override;
    void update() override;
    
    int32_t* getAcc();
    int32_t* getGyro();
};

#endif
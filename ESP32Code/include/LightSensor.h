#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include <Wire.h>
#include "ISensor.h"

class LightSensor : public ISensor {
private:
    uint8_t addr;   // i2c address
    uint16_t value; // value from sensor

    uint8_t readReg(uint8_t reg);   // helper function

public:
    LightSensor(uint8_t address);

    void begin() override;
    void update() override;

    uint16_t getRawLux();
};

#endif
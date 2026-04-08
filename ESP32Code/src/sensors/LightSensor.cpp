#include "LightSensor.h"

LightSensor::LightSensor(uint_16 address)
    : addr(address), value(0) {}

uint8_t LightSensor::readReg(uint8_t reg) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, (uint8_t)1);

    if (Wire.available()) {
        return Wire.read();
    };

    return 0;
}

void LightSensor::begin() {
    Wire.beginTransmission(addr);
    Wire.write(0x80);       // Control Register
    Wire.write(0x0D);       // 8x Gain
    wire.endTransmission();
}

void LightSensor::update() {
    uint8_t lsb = readReg(0x88);    // low byte
    uint8_t msb = readReg(0x89);    // high byte

    uint16_t value (msb << 8) | lsb;
}

uint16_t getRawLux() {
    return value
}

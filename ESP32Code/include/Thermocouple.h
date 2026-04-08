#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include <Arduino.h>
#include "Adafruit_MAX31855.h"
#include "ISensor.h"

class Thermocouple : public ISensor {
private:
    Adafruit_MAX31855 tc;
    double temperature;

public:
    Thermocouple(int clk, int cs, int miso);

    void begin() override;
    void update() override;

    double getTemperature();
};

#endif
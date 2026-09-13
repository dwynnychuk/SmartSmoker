#include "Thermocouple.h"

Thermocouple::Thermocouple(int clk, int cs, int miso)
    : tc(clk, cs, miso), temperature(0) {}

bool Thermocouple::begin() {
    return tc.begin();
}

void Thermocouple::update() {
    double t = tc.readCelsius();

    if(isnan(t)) {
        uint8_t err = tc.readError();

        if (err & MAX31855_FAULT_OPEN) temperature = -1;
        else if (err & MAX31855_FAULT_SHORT_GND) temperature = -1;
        else if (err & MAX31855_FAULT_SHORT_VCC) temperature = -1;
        else temperature = -1;
    } else {
        temperature = t;
    }
}

double Thermocouple::getTemperature() {
    return temperature;
}
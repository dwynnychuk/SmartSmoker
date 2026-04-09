#ifndef RTD_H
#define RTD_H

#include <Arduino.h>
#include "ISensor.h"

class RTD : public ISensor {
private:
    int pin_plus;
    int pin_minus;

    float v_ref;
    float r_ref;

    float temperature;
    float voltage;

    float readVoltage(int pin);
    float voltageToResistance(float v);
    float ResistanceToTemperature(float r);

public: 
    RTD(int analog_pin_plus, int analog_pin_minus, float v_ref, float r_ref);

    void begin() override;
    void update() override;

    float getTemperature();
    float getVoltage();
};


#endif
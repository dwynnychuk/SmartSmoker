#ifndef RTD_H
#define RTD_H

#include <Arduino.h>
#include "ISensor.h"

class RTD : public ISensor {
private:
    int pinPlus;
    int pinMinus;

    float vref;
    float rref;

    float temperature;
    float voltage;

    float readVoltage(int pin);
    float voltageToResistance(float v);
    float ResistanceToTemperature(float r);

public: 
    RTD(int analogPinPlus, int analogPinMinus, float vref, float rref);

    void begin() override;
    void update() override;

    float getTemperature();
    float getVoltage();
};


#endif
#ifndef RTD_H
#define RTD_H

#include <Arduino.h>
#include "ISensor.h"

class RTD : public ISensor {
private:
    int pin;;

    float vref;
    float rref;

    float temperature;
    float voltage;

    float readVoltage();
    float voltageToResistance(float v);
    float ResistanceToTemperature(float r);

public: 
    RTD(int analogPin, float vref, float rref);

    void begin() override;
    void update() override;

    float getTemperature();
    float getVoltage();
};


#endif
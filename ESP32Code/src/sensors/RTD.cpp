#include "RTD.h"

RTD::RTD(int analogPinPlus, int analogPinMinus, float vref, float rref)
    : pinPlus(analogPinPlus), pinMinus(analogPinMinus), vref(vref), rref(rref),
    temperature(0), voltage(0) {}

void RTD::begin() {
    pinMode(pinPlus, INPUT);
    pinMode(pinMinus, INPUT);
}

float RTD::readVoltage(int pin) {
    int raw = analogRead(pin);
    return (raw / 4095.0) * vref; // 12bit
}

float RTD::voltageToResistance(float v_out) {
    float x = 0.5 - (v_out / vref);

    // precent dividing by zero
    if (x < 0.01f) x = 0.01f;
    if (x > 0.99f) x = 0.99f;

    return rref * (x / (1.0f - x));
}

void RTD::update() {
    //
}

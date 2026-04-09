#include "RTD.h"

RTD::RTD(int analog_pin_plus, int analog_pin_minus, float v_ref, float r_ref)
    : pin_plus(analog_pin_plus), pin_minus(analog_pin_minus), v_ref(v_ref), r_ref(r_ref),
    temperature(0), voltage(0) {}

void RTD::begin() {
    pinMode(pin_plus, INPUT);
    pinMode(pin_minus, INPUT);
}

float RTD::readVoltage(int pin) {
    int raw = analogRead(pin);
    return (raw / 4095.0) * v_ref; // 12bit
}

float RTD::voltageToResistance(float v_out) {
    float x = 0.5 - (v_out / v_ref);

    // precent dividing by zero
    if (x < 0.01f) x = 0.01f;
    if (x > 0.99f) x = 0.99f;

    return r_ref * (x / (1.0f - x));
}

void RTD::update() {
    float v_plus = readVoltage(pin_plus);
    float v_minus = readVoltage(pin_minus);

    float v_out = v_plus - v_minus;
    voltage = v_out;

    float resistance = voltageToResistance(v_out);

    temperature = (resistance - 100.0f) / 0.385f; //pt 100 estimation
}

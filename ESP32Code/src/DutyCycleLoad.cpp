#include "DutyCycleLoad.h"

DutyCycleLoad::DutyCycleLoad(uint8_t pin, uint32_t period_ms)
    : _pin(pin), _period_ms(period_ms) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void DutyCycleLoad::set(float value) {
    _duty = constrain(value, 0.0f, 1.0f);
}

void DutyCycleLoad::tick() {
    uint32_t elapsed = (millis() - _period_start) % _period_ms;
    uint32_t on_time = (uint32_t)(_duty * _period_ms);
    bool should_be_on = (elapsed < on_time);
    if (should_be_on != _relay_state) {
        _relay_state = should_be_on;
        digitalWrite(_pin, _relay_state ? HIGH : LOW);
    }
}
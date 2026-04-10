#include "IgnitorController.h"

IgnitorController::IgnitorController(uint8_t pin)
    : _pin(pin) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void IgnitorController::set(float value) {
    if (value > 0.0f) {
        if (!_on) {
            _on = true;
            _on_since = millis();
            digitalWrite(_pin, HIGH);
        } 
    } else {
            off();
    }
}

void IgnitorController::tick() {
    if (_on && (millis() - _on_since) >= MAX_ON_MS) {
        off();
    }
}

void IgnitorController::off() {
    _on = false;
    digitalWrite(_pin, LOW);
}

bool IgnitorController::timed_out() const {
    return _on && (millis() - _on_since) >= MAX_ON_MS;
}
#include "EncoderInput.h"

// Static pointer so free functions can reach the instance from ISR context
static EncoderInput* _instance = nullptr;

static void IRAM_ATTR _isrCLKHandler() {
    if (_instance) _instance->_isrCLK();
}

static void IRAM_ATTR _isrSWHandler() {
    if (_instance) _instance->_isrSW();
}

EncoderInput::EncoderInput(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW)
    : _pinCLK(pinCLK), _pinDT(pinDT), _pinSW(pinSW) {}

void EncoderInput::begin() {
    pinMode(_pinCLK, INPUT_PULLUP);
    pinMode(_pinDT,  INPUT_PULLUP);
    pinMode(_pinSW,  INPUT_PULLUP);

    _instance = this;

    attachInterrupt(digitalPinToInterrupt(_pinCLK), _isrCLKHandler, FALLING);
    attachInterrupt(digitalPinToInterrupt(_pinSW),  _isrSWHandler,  FALLING);
}

EncoderEvent EncoderInput::read() {
    uint32_t now = millis();

    // check hold
    bool holdFired = false;
    if (_swDown & !_holdFired & (now - _swDownMs) >= HOLD_MS) {
        holdFired = true;
        _holdFired = true;  // prevent re-fire
    }

    noInterrupts();
    EncoderEvent evt;
    evt.delta   = _delta;
    evt.hold    = holdFired;
    evt.pressed = _pressed & !holdFired;
    _delta   = 0;
    _pressed = false;
    interrupts();
    return evt;
}

void EncoderInput::_isrCLK() {
    uint32_t now = millis();

    // Ignore bounces within debounce window
    if ((now - _lastCLKMs) < DEBOUNCE_MS) return;
    _lastCLKMs = now;

    if (digitalRead(_pinDT) == HIGH)
        _delta++;
    else
        _delta--;
}

void EncoderInput::_isrSW() {
    uint32_t now = millis();

    if ((now - _lastSWMs) < SW_DEBOUNCE_MS) return;
    _lastSWMs = now;

    if (digitalRead(_pinSW) == LOW) {
        //button going down
        _swDown = true;
        _swDownMs = now;
    } else {
        // button released
        if (!_holdFired)
            _pressed = true;
        _swDown = false;
        _holdFired = false;
    }
}
#ifndef ENCODERINPUT_H
#define ENCODERINPUT_H
#include <Arduino.h>
#include "EncoderEvent.h"

class EncoderInput {
public:
    EncoderInput(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW);

    void begin();
    EncoderEvent read();

    void _isrCLK();
    void _isrSW();

private:
    uint8_t _pinCLK;
    uint8_t _pinDT;
    uint8_t _pinSW;

    volatile int8_t _delta         = 0;
    volatile bool _pressed         = false;
    volatile uint32_t _lastCLKMs   = 0;   // debounce timestamp for rotation
    volatile uint32_t _lastSWMs    = 0;   // debounce timestamp for button
    volatile uint32_t _swDownMs    = 0;   // when button is pressed
    volatile bool _swDown          = false; // is button held

    bool _holdFired = false; // prevent multiple fires

    static constexpr uint32_t DEBOUNCE_MS = 5;    // rotation debounce
    static constexpr uint32_t SW_DEBOUNCE_MS = 50; // button debounce
    static constexpr uint32_t HOLD_MS = 3000; // trigger hold

};
#endif
#ifndef ENCODERINPUT_H
#define ENCODERINPUT_H
#include <Arduino.h>
#include "EncoderEvent.h"

class EncoderInput {
public:
    EncoderInput(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW);

    void begin();
    EncoderEvent read();

    void isrCLK();
    void isrSW();

private:
    uint8_t _pinCLK;
    uint8_t _pinDT;
    uint8_t _pinSW;

    volatile int8_t _delta         = 0;
    volatile bool _pressed         = false;
    volatile uint32_t _lastCLKMs   = 0;   // debounce timestamp for rotation
    volatile uint32_t _lastSWMs    = 0;   // debounce timestamp for button

    static constexpr uint32_t DEBOUNCE_MS = 5;    // rotation debounce
    static constexpr uint32_t SW_DEBOUNCE_MS = 50; // button debounce

};
#endif
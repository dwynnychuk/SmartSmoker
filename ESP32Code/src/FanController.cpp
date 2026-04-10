#include "FanController.h"

FanController::FanController(uint8_t pin, uint32_t period_ms)
    : DutyCycleLoad(pin, period_ms) {}
#include "AugerController.h"

AugerController::AugerController(uint8_t pin, uint32_t period_ms)
    : DutyCycleLoad(pin, period_ms) {}


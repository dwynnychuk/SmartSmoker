#ifndef AUGERCONTROLLER_H
#define AUGERCONTROLLER_H
#include "DutyCycleLoad.h"

class AugerController : public DutyCycleLoad {
public:
    AugerController(uint8_t pin, uint32_t period_ms = 20000);
};

#endif
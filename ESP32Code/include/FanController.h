#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H
#include "DutyCycleLoad.h"

class FanController : public DutyCycleLoad {
public:
    FanController(uint8_t pin, uint32_t period_ms = 20000);
};

#endif
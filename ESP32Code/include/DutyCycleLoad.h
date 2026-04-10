#ifndef DUTYCYCLELOAD_H
#define DUTYCYCLELOAD_H
#include "ILoad.h"

class DutyCycleLoad : public ILoad {
public: 
    DutyCycleLoad(uint8_t pin, uint32_t period_ms);
    void set(float value) override;
    void tick() override;

private:
    uint8_t     _pin;
    uint32_t    _period_ms;
    uint32_t    _period_start = 0;
    float       _duty = 0;
    bool        _relay_state = false;
};

#endif
#ifndef IGNITORCONTROLLER_H
#define IGNITORCONTROLLER_H
#include "ILoad.h"

class IgnitorController : public ILoad {
public:
    IgnitorController(uint8_t pin);
    void set(float value) override;
    void tick() override;
    void off() override;
    bool timed_out() const;

private:
    uint8_t     _pin;
    uint32_t    _on_since = 0;
    bool        _on = false;
    static constexpr uint32_t MAX_ON_MS = 7UL * 60 * 1000;
};

#endif
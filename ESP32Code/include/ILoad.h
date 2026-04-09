#ifndef ILOAD_H
#define ILOAD_H

#include <Arduino.h>

class ILoad {
public:
    virtual ~ILoad() = default;
    virtual void set(float value) = 0;  // 0.0 off, 1.0 on
    virtual void tick() = 0;            // called every loop
    virtual void off() {set(0.0f);}
};

#endif
#ifndef ISENSOR_H
#define ISENSOR_H

class ISensor {
public:
    virtual void begin() = 0;
    virtual void update() = 0;
};

#endif
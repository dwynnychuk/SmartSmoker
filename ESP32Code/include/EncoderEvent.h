#ifndef ENCODEREVENT_H
#define ENCODEREVENT_H
#include <stdint.h>

struct EncoderEvent {
    int8_t delta;
    bool   pressed;
    bool   hold;
};

#endif
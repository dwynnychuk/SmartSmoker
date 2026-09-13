#ifndef GRILLTYPES_H
#define GRILLTYPES_H
#include <stdint.h>

enum class GrillState : uint8_t {
    IDLE,
    IGNITION,
    PREHEAT,
    TEMP_HOLD,
    LID_OPEN,
    COOLDOWN,
    ERROR
};

enum class ErrorCode : uint8_t {
    NONE,
    IGNITION_TIMEOUT,
    OVERTEMP,
    UNDERTEMP,
    SENSOR_FAULT
};

#endif
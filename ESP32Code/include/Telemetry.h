#ifndef TELEMETRY_H
#define TELEMETRY_H
#include <stdint.h>

struct Telemetry {
    // Temperatures
    float tempRTD = 0.0f;
    float tempProbe = 0.0f;

    // Ambient Light Sensor
    uint16_t ambientRaw = 0;

    // IMU
    int32_t accel[3] = {};
    int32_t gyro[3] = {};

    // Load States
    float augerDuty = 0.0f;
    float ignitorDuty = 0.0f;
    float fanDuty = 0.0f;

    // Time
    uint32_t timestamp = 0;

    // Flags
    bool lidOpen = false;

};

#endif 
#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <Preferences.h>   // ESP32 NVS for calibration storage
#include "Telemetry.h"
#include "IMU.h"
#include "LightSensor.h"
#include "RTD.h"
#include "Thermocouple.h"

class SensorManager {
public:
    SensorManager (IMU& imu,
                   LightSensor& lightsensor,
                   RTD& rtd,
                   Thermocouple& thermocouple);

    bool begin();       // init all sensors
    void update();      // tick all sensors
    const Telemetry& getData() const { return _data; }

    void calibrateLidClosed();
    bool isCalibrated() const { return _lidCalibrated; }

private:
        void _updateTemps();
        void _updateIMU();
        void _updateLight();
        void _computeLidOpen();

        float _dotProduct(const int32_t a[3], const int32_t b[3]) const;
        float _magnitude(const int32_t v[3]) const;

        IMU& _imu;
        LightSensor& _lightsensor;
        RTD& _rtd;
        Thermocouple& _thermocouple;
        Telemetry& _data;

        // lid calibration
        int32_t _lidClosedRef[3] = {};
        bool _lidCalibrated = false;
        Preferences _prefs;

        static constexpr float LID_ANGLE_THRES = 0.85f; // cos(32)
};

#endif
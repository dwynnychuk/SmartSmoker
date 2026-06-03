#include "SensorManager.h"
#include <math.h>

SensorManager::SensorManager(IMU& imu, LightSensor& light, 
    RTD& rtd, Thermocouple& thermocouple)
    : _imu(imu), _lightsensor(light), _rtd(rtd), _thermocouple(thermocouple) {};

bool SensorManager::begin() {
    bool ok = true;
    ok &= _imu.begin();
    ok &= _lightsensor.begin();
    ok &= _rtd.begin();
    ok &= _thermocouple.begin();

    // Load calibration from NVS if it exists
    _prefs.begin("smoker", true);
    if (_prefs.isKey("calX")) {
        _lidClosedRef[0] = _prefs.getInt("calX");
        _lidClosedRef[1] = _prefs.getInt("calY");
        _lidClosedRef[2] = _prefs.getInt("calZ");
        _lidCalibrated = true;
    }
    _prefs.end();

    return ok;
}

void SensorManager::update() {
    _updateTemps();
    _updateIMU();
    _updateLight();
    _computeLidOpen();
    _data.timestamp = millis();
}

void SensorManager::_updateTemps() {
    _rtd.update();
    _thermocouple.update();
    _data.tempRTD = _rtd.getTemperature();
    _data.tempProbe = _thermocouple.getTemperature();
}

void SensorManager::_updateIMU() {
    _imu.update();
    _data.accel = _imu.getAcc();
    _data.gyro = _imu.getGyro();
}

void SensorManager::_updateLight() {
    _lightsensor.update();
    _data.ambientRaw = _lightsensor.getRawLux();
}

void SensorManager::_computeLidOpen() {
    if (!_lidCalibrated) {
        _data.lidOpen = false;
        return;
    }

    // Dot product of current accel vs closed reference
    // If cosine of angle drops below threshold, lid has rotated open
    float dot  = _dotProduct(_data.accel, _lidClosedRef);
    float magA = _magnitude(_data.accel);
    float magB = _magnitude(_lidClosedRef);

    if (magA < 1.0f || magB < 1.0f) {
        _data.lidOpen = false;  // bad reading, don't false-trigger
        return;
    }

    float cosAngle = dot / (magA * magB);

    _data.lidOpen = cosAngle < LID_ANGLE_THRES;
}

void SensorManager::calibrateLidClosed() {
    // snapshot lid closed as reference
    _lidClosedRef[0] = _data.accel[0];
    _lidClosedRef[1] = _data.accel[1];
    _lidClosedRef[2] = _data.accel[2];
    _lidCalibrated = true;

    // write to nvs
    _prefs.begin("smoker", false);
    _prefs.putInt("calX", _lidClosedRef[0]);
    _prefs.putInt("calY", _lidClosedRef[1]);
    _prefs.putInt("calZ", _lidClosedRef[2]);
    _prefs.end(); 
}

float SensorManager::_dotProduct(const int32_t a[3], const int32_t b[3]) const {
    return (float)a[0]*b[0] + (float)a[1]*b[1] + (float)a[2]*b[2];
}

float SensorManager::_magnitude(const int32_t v[3]) const {
    return sqrtf((float)v[0]*v[0] + (float)v[1]*v[1] + (float)v[2]*v[2]);
}
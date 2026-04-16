#include "PID.h"

PID::PID(float kp, float ki, float kd)
    : _kp(kp), _ki(ki), _kd(kd) {}

float PID::_compute_p(float error) {
    return _kp*error;
};

float PID::_compute_i(float error, float dt) {
    if (!_paused) {
        _accumulated_error += error * dt;
        _accumulated_error = constrain(_accumulated_error, -INTEGRAL_MAX, INTEGRAL_MAX);
    }
    return _accumulated_error;
}

float PID::_compute_d(float error, float dt) {
    if (dt <= 0.0f) return 0.0f;

    return _kd * (error - _previous_error) / dt;
}

float PID::compute_pid(float set_temp, float measured) {
    float error = set_temp - measured;
    float dt = (millis() - _last_compute_ms)/MS_TO_S;
    float output = _compute_p(error) + _compute_i(error, dt) + _compute_d(error, dt);
    output = constrain(output, OUTPUT_MIN, OUTPUT_MAX);
}
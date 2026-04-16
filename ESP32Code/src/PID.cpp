#include "PID.h"

PID::PID(float kp, float ki, float kd)
    : _kp(kp), _ki(ki), _kd(kd) {}

float PID::_compute_p(float error) {
    return _kp*error;
}

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
    uint32_t now = millis();
    if ((now - _last_compute_ms) >= UPDATE_INTERVAL_MS) {
        float error = set_temp - measured;
        float dt    = (now - _last_compute_ms)/MS_TO_S;
        _last_output = _compute_p(error)
                + _compute_i(error, dt) 
                + _compute_d(error, dt);
        _last_output = constrain(_last_output, OUTPUT_MIN, OUTPUT_MAX);
        _previous_error = error;
        _last_compute_ms = now;
    };
    return _last_output;
}

void PID::pause_integral() {
    _paused = true;
}

void PID::resume_integral() {
    _paused = false;
}

void PID::reset() {
    _last_compute_ms = 0;
    _accumulated_error = 0;
    _previous_error = 0;
    _last_output = 0.0f;
    _paused = false;
}
#ifndef PID_H
#define PID_H
#include <Arduino.h>

class PID{
public:
    PID(float kp, float ki, float kd);
    float compute_pid(float set_temp, float measured);
    void pause_integral();
    void resume_integral();
    void reset();

private:
    float _compute_p(float error);
    float _compute_d(float error, float dt);
    float _compute_i(float error, float dt);

    uint32_t    _last_compute_ms = 0;
    float       _kp;
    float       _ki;
    float       _kd;
    float       _accumulated_error = 0;
    float       _previous_error = 0;
    float       _last_output = 0.0f;
    bool        _paused = false;

    static constexpr uint32_t UPDATE_INTERVAL_MS = 20000;
    static constexpr float OUTPUT_MIN = 0.02;
    static constexpr float OUTPUT_MAX = 1.0;
    static constexpr float INTEGRAL_MAX = 0.75;
    static constexpr float MS_TO_S = 1000.0f;
};

#endif
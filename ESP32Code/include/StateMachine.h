#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <Arduino.h>
#include "Telemetry.h"
#include "AugerController.h"
#include "FanController.h"
#include "IgnitorController.h"
#include "PID.h"
#include "DisplayController.h"

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

struct EncoderEvent {
    int8_t delta;
    bool   pressed;
    bool   hold;
};

class StateMachine {
public:
    StateMachine(AugerController&   auger,
                FanController&     fan,
                IgnitorController& ignitor,
                PID&               pid,
                DisplayController&    display);

    void tick(const Telemetry& data, const EncoderEvent& enc);

    GrillState  getState()      const { return _state; }
    ErrorCode   getError()      const { return _error; }
    float       getSetTemp()    const { return _setTemp; }

private:
    void _transitionTo(GrillState next, ErrorCode fault = ErrorCode::NONE);

    // Per-state handlers
    void _tickIDLE     (const Telemetry& d, const EncoderEvent& enc);
    void _tickIGNITION (const Telemetry& d, const EncoderEvent& enc);
    void _tickPREHEAT  (const Telemetry& d, const EncoderEvent& enc);
    void _tickTEMP_HOLD(const Telemetry& d, const EncoderEvent& enc);
    void _tickLID_OPEN (const Telemetry& d, const EncoderEvent& enc);
    void _tickCOOLDOWN (const Telemetry& d, const EncoderEvent& enc);
    void _tickERROR    (const Telemetry& d, const EncoderEvent& enc);

    // Guards
    bool _lidIsOpen  (const Telemetry& d) const;
    bool _tempRisen  (const Telemetry& d) const;
    bool _nearSetTemp(const Telemetry& d) const;
    bool _flameout   (const Telemetry& d) const;
    bool _overTemp   (const Telemetry& d) const;
    bool _sensorFault(const Telemetry& d) const;
    bool _grillCool  (const Telemetry& d) const;

    uint32_t _timeInState() const { return millis() - _stateEnteredMs; }

    // Loads + logic
    AugerController&   _auger;
    FanController&     _fan;
    IgnitorController& _ignitor;
    PID&               _pid;
    DisplayController& _display;

    // State
    GrillState  _state = GrillState::IDLE;
    ErrorCode   _error = ErrorCode::NONE;
    float       _setTemp = 225.0f;
    float       _tempAtIgnition = 0.0f;
    uint32_t    _stateEnteredMs = 0;

    // Tuning Config
    static constexpr float    IGNITE_RISE      = 20.0f;
    static constexpr uint32_t IGNITE_TIMEOUT_MS  = 30UL * 60 * 1000;
    static constexpr float    PREHEAT_WINDOW   = 10.0f;
    static constexpr float    UNDERTEMP_DELTA  = 25.0f;
    static constexpr uint32_t UNDERTEMP_HOLD_MS  = 60000;
    static constexpr float    OVERTEMP_MAX     = 50.0f;
    static constexpr float    TARGET_MIN       = 180.0f;
    static constexpr float    TARGET_MAX       = 400.0f;
    static constexpr float    TARGET_STEP      = 5.0f;
    static constexpr float    COOLDOWN_TARGET  = 120.0f;

    // Fan/auger fixed rates per state
    static constexpr float IGNITION_AUGER = 0.30f;
    static constexpr float IGNITION_FAN   = 0.80f;
    static constexpr float COOLDOWN_AUGER = 0.05f;
    static constexpr float COOLDOWN_FAN   = 1.00f;
    static constexpr float IGNITOR_CYCLE  = 0.70f;
};

#endif
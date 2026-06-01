#include "StateMachine.h"

StateMachine::StateMachine(AugerController&   auger,
                            FanController&     fan,
                            IgnitorController& ignitor,
                            PID&               pid,
                            DisplayController&    display)
    : _auger(auger), _fan(fan), _ignitor(ignitor), _pid(pid), _display(display) {}

// ── Public tick ───────────────────────────────────────────────────────────────

void StateMachine::tick(const Telemetry& data, const EncoderEvent& enc) {

    // Global safety guards — skip in states that already handle everything off
    if (_state != GrillState::IDLE    &&
        _state != GrillState::ERROR   &&
        _state != GrillState::COOLDOWN) {
        if (_sensorFault(data))
            return _transitionTo(GrillState::ERROR, ErrorCode::SENSOR_FAULT);
        if (_overTemp(data))
            return _transitionTo(GrillState::ERROR, ErrorCode::OVERTEMP);
    }

    // Dispatch to per-state handler
    switch (_state) {
        case GrillState::IDLE:      _tickIDLE       (data, enc);   break;
        case GrillState::IGNITION:  _tickIGNITION   (data, enc);   break;
        case GrillState::PREHEAT:   _tickPREHEAT    (data, enc);   break;
        case GrillState::TEMP_HOLD: _tickTEMP_HOLD  (data, enc);   break;
        case GrillState::LID_OPEN:  _tickLID_OPEN   (data, enc);   break;
        case GrillState::COOLDOWN:  _tickCOOLDOWN   (data, enc);   break;
        case GrillState::ERROR:     _tickERROR      (data, enc);   break;
    }

    // Tick all loads every loop
    _auger.tick();
    _fan.tick();
    _ignitor.tick();

    // Display always reflects current state
    _display.render(_state, data, _setTemp, _error);
}

// ── Transition ────────────────────────────────────────────────────────────────

void StateMachine::_transitionTo(GrillState next, ErrorCode fault) {
    // Exit actions
    switch (_state) {
        case GrillState::LID_OPEN:
            _pid.resume_integral();
            break;
        default:
            break;
    }
    
    _error          = fault;
    _state          = next;
    _stateEnteredMs = millis();

    // Entry actions
    switch (next) {
        case GrillState::IDLE:
            _error = ErrorCode::NONE;
            _ignitor.off();
            _auger.off();
            _fan.off();
            break;

        case GrillState::IGNITION:
            _tempAtIgnition = 0.0f;   // snapshot on first valid tick
            _pid.reset();
            _ignitor.set(IGNITOR_CYCLE);
            _auger.set(IGNITION_AUGER);
            _fan.set(IGNITION_FAN);
            break;

        case GrillState::PREHEAT:
            _pid.reset();
            break;

        case GrillState::TEMP_HOLD:
            _ignitor.off();
            break;

        case GrillState::LID_OPEN:
            _pid.pause_integral();   // prevent windup while lid is open
            break;

        case GrillState::COOLDOWN:
            _ignitor.off();
            _auger.set(COOLDOWN_AUGER);
            _fan.set(COOLDOWN_FAN); // fan runs until cool
            break;

        case GrillState::ERROR:
            _ignitor.off();
            _auger.off();
            _fan.off();
            break;
    }
}

// ── Per-state tick handlers ───────────────────────────────────────────────────

void StateMachine::_tickIDLE(const Telemetry& d, const EncoderEvent& enc) {
    if (enc.delta != 0)
        _setTemp = constrain(_setTemp + enc.delta * TARGET_STEP,
                                TARGET_MIN, TARGET_MAX);
    if (enc.pressed)
        _transitionTo(GrillState::IGNITION);
}

void StateMachine::_tickIGNITION(const Telemetry& d, const EncoderEvent& enc) {
    // Snapshot ambient temp on first valid RTD reading
    if (_tempAtIgnition == 0.0f && d.tempRTD > 0.0f)
        _tempAtIgnition = d.tempRTD;

    if (_tempRisen(d))
        return _transitionTo(GrillState::PREHEAT);

    if (_timeInState() >= IGNITE_TIMEOUT_MS)
        return _transitionTo(GrillState::ERROR, ErrorCode::IGNITION_TIMEOUT);

    // Allow manual abort by holding encoder
    if (enc.hold)
        _transitionTo(GrillState::COOLDOWN);
}

void StateMachine::_tickPREHEAT(const Telemetry& d, const EncoderEvent& enc) {
    if (_nearSetTemp(d))
        return _transitionTo(GrillState::TEMP_HOLD);

    if (enc.hold)
        _transitionTo(GrillState::COOLDOWN);
}

void StateMachine::_tickTEMP_HOLD(const Telemetry& d, const EncoderEvent& enc) {
    // PID drives auger, fan follows
    float duty = _pid.compute_pid(d.tempRTD, _setTemp);
    _auger.set(duty);
    _fan.set(1.0f);   // fan tracks auger: min 40%, max 100%

    if (_lidIsOpen(d))
        return _transitionTo(GrillState::LID_OPEN);

    if (_flameout(d))
        return _transitionTo(GrillState::ERROR, ErrorCode::UNDERTEMP);

    // Live temp adjustment with encoder
    if (enc.delta != 0)
        _setTemp = constrain(_setTemp + enc.delta * TARGET_STEP,
                                TARGET_MIN, TARGET_MAX);
    if (enc.hold)
        _transitionTo(GrillState::COOLDOWN);
}

void StateMachine::_tickLID_OPEN(const Telemetry& d, const EncoderEvent& enc) {
    if (!_lidIsOpen(d)) {
        return _transitionTo(GrillState::TEMP_HOLD);
    }
    if (enc.hold)
        _transitionTo(GrillState::COOLDOWN);
}

void StateMachine::_tickCOOLDOWN(const Telemetry& d, const EncoderEvent& enc) {
    if (_grillCool(d))
        _transitionTo(GrillState::IDLE);
}

void StateMachine::_tickERROR(const Telemetry& d, const EncoderEvent& enc) {
    if (enc.pressed)
        _transitionTo(GrillState::IDLE);
}

// ── Guards ────────────────────────────────────────────────────────────────────

bool StateMachine::_lidIsOpen(const Telemetry& d) const {
    return (d.ambientRaw > LID_LIGHT_THRESH) && d.lidOpen;
}

bool StateMachine::_tempRisen(const Telemetry& d) const {
    if (_tempAtIgnition == 0.0f) return false;
    return (d.tempRTD - _tempAtIgnition) >= IGNITE_RISE;
}

bool StateMachine::_nearSetTemp(const Telemetry& d) const {
    return d.tempRTD >= (_setTemp - PREHEAT_WINDOW);
}

bool StateMachine::_flameout(const Telemetry& d) const {
    return (d.tempRTD < (_setTemp - UNDERTEMP_DELTA))
           && (_timeInState() > UNDERTEMP_HOLD_MS);
}

bool StateMachine::_overTemp(const Telemetry& d) const {
    return d.tempRTD > (TARGET_MAX + OVERTEMP_MAX);
}

bool StateMachine::_sensorFault(const Telemetry& d) const {
    return (d.tempRTD <= 0.0f) || (d.tempProbe < 0.0f);
}

bool StateMachine::_grillCool(const Telemetry& d) const {
    return (d.tempRTD > 0.0f) && (d.tempRTD < COOLDOWN_TARGET);
}

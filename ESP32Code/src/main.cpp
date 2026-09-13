#include <Arduino.h>
#include <Wire.h>

#include "Telemetry.h"
#include "GrillTypes.h"
#include "EncoderEvent.h"

#include "IMU.h"
#include "LightSensor.h"
#include "RTD.h"
#include "Thermocouple.h"
#include "SensorManager.h"

#include "AugerController.h"
#include "FanController.h"
#include "IgnitorController.h"
#include "PID.h"

#include "DisplayController.h"
#include "EncoderInput.h"
#include "StateMachine.h"

// ── Pin assignments ─────────────
#define PIN_AUGER        25
#define PIN_FAN          26
#define PIN_IGNITOR      27

#define PIN_RTD_PLUS     34   // ADC1 pin
#define PIN_RTD_MINUS    35   // ADC1 pin
#define RTD_VREF         3.3f
#define RTD_RREF         1000.0f

#define TC_CLK           18
#define TC_CS            5
#define TC_MISO          19

#define IMU_I2C_ADDR     0xD7   // LSM6DSR address

#define LIGHT_I2C_ADDR   0x29   // LTR-329 address

#define PIN_ENC_CLK      32
#define PIN_ENC_DT       33
#define PIN_ENC_SW       14

// PID tuning — placeholder values
#define PID_KP           0.02f
#define PID_KI           0.0005f
#define PID_KD           0.01f

// ── Shared state ─────────────────────────────────────────────────────────
Telemetry telemetry;

// ── Sensors ──────────────────────────────────────────────────────────────
IMU          imu(&Wire, IMU_I2C_ADDR);
LightSensor  lightSensor(LIGHT_I2C_ADDR);
RTD          rtd(PIN_RTD_PLUS, PIN_RTD_MINUS, RTD_VREF, RTD_RREF);
Thermocouple thermocouple(TC_CLK, TC_CS, TC_MISO);

SensorManager sensors(imu, lightSensor, rtd, thermocouple, telemetry);

// ── Loads ────────────────────────────────────────────────────────────────
AugerController   auger(PIN_AUGER);
FanController     fan(PIN_FAN);
IgnitorController ignitor(PIN_IGNITOR);

// ── Control / UI ─────────────────────────────────────────────────────────
PID               pid(PID_KP, PID_KI, PID_KD);
DisplayController display;
EncoderInput      encoder(PIN_ENC_CLK, PIN_ENC_DT, PIN_ENC_SW);

StateMachine stateMachine(auger, fan, ignitor, pid, display);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!sensors.begin()) {
        Serial.println("WARNING: one or more sensors failed to init");
        // Intentionally continue rather than halt — StateMachine's
        // _sensorFault() guard will catch bad readings and go to ERROR.
    }

    if (!display.begin()) {
        Serial.println("WARNING: display failed to init");
    }

    encoder.begin();

    Serial.println("Smoker controller ready.");
}

void loop() {
    sensors.update();
    EncoderEvent enc = encoder.read();

    stateMachine.tick(sensors.getData(), enc);

    // Small delay to avoid hammering I2C/ADC every cycle.
    // UPDATE_INTERVAL_MS gate so this doesn't need to be precise.
    delay(50);
}
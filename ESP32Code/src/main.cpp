#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <vl53lx_class.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "Adafruit_MAX31855.h"
#include <LSM6DSRSensor.h>

#define LED 17  // Internal LED
#define MPA0 2  // A0 for Multiplexer
#define MPA1 15 // A1 for Multiplexer
#define cs 5    // SPI CS
#define clk 18  // SPI SCK
#define miso 19 // SPI MISO

#define DEV_I2C Wire

const int LSM_IMU_ADDR_LID = 0xD7; // LID IMU + READ
const int LSM_IMU_ADDR_HOP = 0xD5; // Hopper IMU + READ

const int LTR_329_ADDR = 0x29;  // LID Light Sensor
const int LTR_REG_CONTR = 0x80; // LTR Control Register
const int LTR_CONTR_VAL = 0x0D; // 8x gain
const int LTR_CH1_LOW = 0x88;
const int LTR_CH1_HIGH = 0x89;
const int LTR_CH0_LOW = 0x8A;
const int LTR_CH0_HIGH = 0x8B;

double ambientC, internalC, foodC1, foodC2; // measured temperatures in celcius

// Initialize thermocouple
Adafruit_MAX31855 thermocouple(clk, cs, miso);

// Initialize IMU
LSM6DSRSensor AccGyrL(&Wire, LSM_IMU_ADDR_LID);
LSM6DSRSensor AccGyrH(&Wire, LSM_IMU_ADDR_HOP);

// Setup
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(MPA0, OUTPUT);
  pinMode(MPA1, OUTPUT);

  Serial.begin(115200);
  Wire.begin();
  while (!Serial)
    delay(10);

  // Light Sensor
  Wire.beginTransmission(LTR_329_ADDR);
  Wire.write(LTR_REG_CONTR);
  Wire.write(LTR_CONTR_VAL);
  Wire.endTransmission();

  // IMU
  AccGyrL.begin();
  AccGyrL.Enable_X();
  AccGyrL.Enable_G();

  AccGyrH.begin();
  AccGyrH.Enable_X();
  AccGyrH.Enable_G();

  // Temperature
  digitalWrite(MPA0, LOW);
  digitalWrite(MPA1, LOW);
  delay(500);
  if (!thermocouple.begin())
  {
    //Serial.print("ERROR");
    while (1)
      delay(10);
  }
  //Serial.print("DONE");
}

// Loop indefinitely
void loop()
{
  // Turn on LED
  digitalWrite(LED, HIGH);

  // Temperature readings
  double ambientC = thermocouple.readCelsius();
  if (isnan(ambientC))
  {
    //Serial.println("Thermocouple Fault:");
    uint8_t therm_error = thermocouple.readError();
    if (therm_error & MAX31855_FAULT_OPEN)
      ambientC = -1;
      //Serial.println("FAULT: Thermocouple is open - no connection.");
    if (therm_error & MAX31855_FAULT_SHORT_GND)
      ambientC = -1;
      //Serial.println("FAULT: Thermocouple is shorted to GND.");
    if (therm_error & MAX31855_FAULT_SHORT_VCC)
      ambientC = -1;
      //Serial.println("FAULT: Thermocouple is shorted to VCC.");
    Serial.print(ambientC);
    Serial.print(", ");
  }
  else
  {
    Serial.print(ambientC);
    Serial.print(", ");
    
  }

  // LID SENSORS
  // Light Sensor (Only available on ch 0)
  byte msb = 0, lsb = 0;
  u_int16_t LTR_CH0_VALUE;

  Wire.beginTransmission(LTR_329_ADDR);
  Wire.write(LTR_CH0_LOW);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
  delay(1);
  if (Wire.available())
    lsb = Wire.read();

  Wire.beginTransmission(LTR_329_ADDR);
  Wire.write(LTR_CH0_HIGH);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
  delay(1);
  if (Wire.available())
    msb = Wire.read();

  LTR_CH0_VALUE = (msb << 8) | lsb;

  Serial.print(LTR_CH0_VALUE, DEC); // output in steps (16bit)
  Serial.print(", ");

  // IMU
  int32_t accelerometerL[3];
  int32_t gyroscopeL[3];
  AccGyrL.Get_X_Axes(accelerometerL);
  AccGyrL.Get_G_Axes(gyroscopeL);

  // IMU Output
  Serial.print(accelerometerL[0]);
  Serial.print(", ");
  Serial.print(accelerometerL[1]);
  Serial.print(", ");
  Serial.print(accelerometerL[2]);
  Serial.print(", ");
  Serial.print(gyroscopeL[0]);
  Serial.print(", ");
  Serial.print(gyroscopeL[1]);
  Serial.print(", ");
  Serial.print(gyroscopeL[2]);
  Serial.print(", ");

  // IMU
  int32_t accelerometerH[3];
  int32_t gyroscopeH[3];
  AccGyrH.Get_X_Axes(accelerometerH);
  AccGyrH.Get_G_Axes(gyroscopeH);

  // IMU Output
  Serial.print(accelerometerH[0]);
  Serial.print(", ");
  Serial.print(accelerometerH[1]);
  Serial.print(", ");
  Serial.print(accelerometerH[2]);
  Serial.print(", ");
  Serial.print(gyroscopeH[0]);
  Serial.print(", ");
  Serial.print(gyroscopeH[1]);
  Serial.print(", ");
  Serial.print(gyroscopeH[2]);
  Serial.println("");
  delay(500);

  // Turn off LED
  digitalWrite(LED, LOW);
  delay(500);

}

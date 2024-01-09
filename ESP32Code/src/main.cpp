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

// BLE Setup
#define BLE_Server_Name "ESP32 Smoker"
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool Device_Connected = false;
bool Old_Device_Connected = false;
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define AccLXCharacteristic_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
// BLE Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Device_Connected = true;
  };
  void onDisconnect(BLEServer *pServer)
  {
    Device_Connected = false;
  }
};
BLECharacteristic AccLXCharacteristic("f78ebbff-c8b7-4107-93de-889a6a06d408", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLEDescriptor AccLXDescriptor(BLEUUID((uint16_t)0x2902));

//BLECharacteristic AccHXCharacteristics("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_NOTIFY);
//BLEDescriptor AccHXDescriptor(BLEUUID((uint16_t)0x2903));


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

  // BLE
  // Create the BLE Device
  BLEDevice::init(BLE_Server_Name);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and Create a BLE Descriptor
  pService->addCharacteristic(&AccLXCharacteristic);
  AccLXDescriptor.setValue("Lid ACC X Value");
  AccLXCharacteristic.addDescriptor(&AccLXDescriptor);

  // Humidity
  //pService->addCharacteristic(&AccHXCharacteristic);
  //AccHXDescriptor.setValue("Hopper ACC X Value");
  //AccHXCharacteristics.addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  //pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  // End of BLE

  // Light Sensor
  Wire.beginTransmission(LTR_329_ADDR);
  Wire.write(LTR_REG_CONTR);
  Wire.write(LTR_CONTR_VAL);
  Wire.endTransmission();

  // ToF Sensor
  // sensor_vl53lx.begin();
  // sensor_vl53lx.VL53LX_Off();
  // sensor_vl53lx.InitSensor(0x12);
  // sensor_vl53lx.VL53LX_StartMeasurement();

  // IMU
  AccGyrL.begin();
  AccGyrL.Enable_X();
  AccGyrL.Enable_G();

  AccGyrH.begin();
  AccGyrH.Enable_X();
  AccGyrH.Enable_G();

  // Temperature
  Serial.println("MAX31855 test");
  digitalWrite(MPA0, LOW);
  digitalWrite(MPA1, LOW);
  delay(500);
  Serial.print("Initializing Sensor...");
  if (!thermocouple.begin())
  {
    Serial.print("ERROR");
    while (1)
      delay(10);
  }
  Serial.print("DONE");
}

// Loop indefinitely
void loop()
{
  // Turn on LED
  digitalWrite(LED, HIGH);

  // Temperature readings
  Serial.print("Temperature #1: ");
  double ambientC = thermocouple.readCelsius();
  if (isnan(ambientC))
  {
    Serial.println("Thermocouple Fault:");
    uint8_t therm_error = thermocouple.readError();
    if (therm_error & MAX31855_FAULT_OPEN)
      Serial.println("FAULT: Thermocouple is open - no connection.");
    if (therm_error & MAX31855_FAULT_SHORT_GND)
      Serial.println("FAULT: Thermocouple is shorted to GND.");
    if (therm_error & MAX31855_FAULT_SHORT_VCC)
      Serial.println("FAULT: Thermocouple is shorted to VCC.");
  }
  else
  {
    Serial.print("C = ");
    Serial.println(ambientC);
  }

  // LID SENSORS
  // Light Sensor
  byte msb = 0, lsb = 0;
  u_int16_t LTR_CH0_VALUE;
  u_int16_t LTR_CH1_VALUE;

  // channel 1
  Wire.beginTransmission(LTR_329_ADDR);
  Wire.write(LTR_CH1_LOW);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
  delay(1);
  if (Wire.available())
    lsb = Wire.read();

  Wire.beginTransmission(LTR_329_ADDR);
  Wire.write(LTR_CH1_HIGH);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
  delay(1);
  if (Wire.available())
    msb = Wire.read();

  LTR_CH1_VALUE = (msb << 8) | lsb;

  // channel 0
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

  Serial.print("Light Sensor CH 0: ");
  Serial.println(LTR_CH1_VALUE, DEC); // output in steps (16bit)
  Serial.print("Light Sensor CH 1: ");
  Serial.println(LTR_CH1_VALUE, DEC); // output in steps (16bit)

  // IMU
  int32_t accelerometerL[3];
  int32_t gyroscopeL[3];
  AccGyrL.Get_X_Axes(accelerometerL);
  AccGyrL.Get_G_Axes(gyroscopeL);

  // IMU Output
  Serial.print("LSM6DSR: | Acc[mg]: ");
  Serial.print(accelerometerL[0]);
  Serial.print(" ");
  Serial.print(accelerometerL[1]);
  Serial.print(" ");
  Serial.print(accelerometerL[2]);
  Serial.print(" | Gyr[mdps]: ");
  Serial.print(gyroscopeL[0]);
  Serial.print(" ");
  Serial.print(gyroscopeL[1]);
  Serial.print(" ");
  Serial.print(gyroscopeL[2]);
  Serial.println(" |");

  // HOPPER SENSORS
  // Light Sensor
  // Not going to populate as I2C address is not selectable

  // ToF Sensor
  // needs development

  // IMU
  int32_t accelerometerH[3];
  int32_t gyroscopeH[3];
  AccGyrH.Get_X_Axes(accelerometerH);
  AccGyrH.Get_G_Axes(gyroscopeH);

  // IMU Output
  Serial.print("LSM6DSR: | Acc[mg]: ");
  Serial.print(accelerometerH[0]);
  Serial.print(" ");
  Serial.print(accelerometerH[1]);
  Serial.print(" ");
  Serial.print(accelerometerH[2]);
  Serial.print(" | Gyr[mdps]: ");
  Serial.print(gyroscopeH[0]);
  Serial.print(" ");
  Serial.print(gyroscopeH[1]);
  Serial.print(" ");
  Serial.print(gyroscopeH[2]);
  Serial.println(" |");
  delay(500);

  // Turn off LED
  digitalWrite(LED, LOW);
  delay(500);

  // BLE
  if (Device_Connected)
  {
    static char AccLXString[6];
    dtostrf(accelerometerL[0], 6, 2, AccLXString);
    // Set temperature Characteristic value and notify connected client
    AccLXCharacteristic.setValue(AccLXString);
    AccLXCharacteristic.notify();
    Serial.print("Acceleration Lid X: ");
    Serial.print(accelerometerL[0]);

    //static char AccHXString[6];
    //dtostrf(accelerometerH[0], 6, 2, AccHXString);
    // Set temperature Characteristic value and notify connected client
    //AccHXCharacteristics.setValue(AccHXString);
    //AccHXCharacteristics.notify();
    //Serial.print("Acceleration Hopper X: ");
    //Serial.print(accelerometerH[0]);
  }
}

/*
// BLE

  BLEDevice::init("ESP32"); // initialize device

  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create BLE Service
  BLEService *pEnvironment = pServer->createService(environmentalService);

  // Create BLE Characteristic
  pEnvironment->addCharacteristic(&temperatureCharacteristic);

  // Create BLE Descriptor
  temperatureCharacteristic.addDescriptor(new BLE2902());
  BLEDescriptor temperatureDescriptor(BLEUUID((uint16_t)0x2901));
  temperatureDescriptor.setValue("Temperature -40 - 40C");
  temperatureCharacteristic.addDescriptor(&temperatureDescriptor);

  // Start the service
  pServer->getAdvertising()->addServiceUUID(environmentalService);
  pEnvironment->start();
  Serial.println("Waiting for a client connection...");

#define environmentalService BLEUUID((uint16_t)0x181A)
BLECharacteristic temperatureCharacteristic(
  BLEUUID((uint16_t)0x2A6E),
  BLECharacteristic::PROPERTY_READ  |
  BLECharacteristic::PROPERTY_NOTIFY
);

int txValue = 0;
bool deviceConnected = false;
class ServerCallbacks: public BLEServerCallbacks{
  void onConnect(BLEServer* pServer){
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};
*/

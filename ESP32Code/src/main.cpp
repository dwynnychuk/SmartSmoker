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

#define LED 17 // Internal LED
#define MPA0 2 // A0 for Multiplexer
#define MPA1 15 // A1 for Multiplexer
#define cs 5 // SPI CS
#define clk 18 // SPI SCK
#define miso 19 // SPI MISO

const int LSM_IMU_ADDR_LID = 0x6B; // LID IMU
const int LSM_IMU_ADDR_HOP = 0x4D; // Hopper IMU

const int LTR_329_ADDR = 0x29; // LID Light Sensor
const int LTR_REG_CONTR = 0x80; // LTR Control Register
const int LTR_CONTR_VAL = 0x0D; // 8x gain
const int LTR_CH1_LOW = 0x88;
const int LTR_CH1_HIGH = 0x89;
const int LTR_CH0_LOW = 0x8A;
const int LTR_CH0_HIGH = 0x8B;

int16_t AccX, AccY, AccZ, Tmp, GyroX, GyroY, GyroZ;
int16_t AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;
float AccAngleX, AccAngleY, AccAngleZ, GyroAngleX, GyroAngleY, GyroAngleZ, BoardTemp;
float ElapsedTime, CurrentTime, PreviousTime;
int v = 0;
float ambientC, internalC, foodC1, foodC2; // measured temperatures in celcius

// SPI reading for temperature
int spiRead() {
  int rawtmp = 0;
  int NC = 0;
  digitalWrite(cs, LOW);
  delay(200);
  digitalWrite(clk, HIGH);
  NC += digitalRead(miso);
  digitalWrite(clk,LOW); 

  // bit 14 - 0
  for (int i = 14; i > 0; i--){
    digitalWrite(clk,HIGH);
    rawtmp += digitalRead(miso) << i;
    digitalWrite(clk,LOW);
  }
  digitalWrite(cs, HIGH);

  if (NC == 0x00) return 0;
  return rawtmp >> 3;
}
float readTemperature();

// Setup
void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  // Light Sensor
  Wire.beginTransmission(LTR_329_ADDR);
  Wire.write(LTR_REG_CONTR);
  Wire.write(LTR_CONTR_VAL);
  Wire.endTransmission();

  // ToF Sensor
  //sensor_vl53lx.begin();
  //sensor_vl53lx.VL53LX_Off();
  //sensor_vl53lx.InitSensor(0x12);
  //sensor_vl53lx.VL53LX_StartMeasurement();
  
  // IMU


  delay(500);
}

// Loop indefinitely 
void loop() {
  // Temperature readings
  /*
  // Temperature 1
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(MPA0, 0);
  digitalWrite(MPA1, 0);
  digitalWrite(LED, HIGH);
  ambientC = readTemperature();
  delay(2000);
  digitalWrite(LED, LOW);

  // Temperature 2
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(MPA0, 1);
  digitalWrite(MPA1, 0);
  digitalWrite(LED, HIGH);
  internalC = readTemperature();
  delay(2000);
  digitalWrite(LED, LOW);

  // Temperature 3
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(MPA0, 0);
  digitalWrite(MPA1, 1);
  digitalWrite(LED, HIGH);
  foodC1 = readTemperature();
  delay(2000);
  digitalWrite(LED, LOW);

  // Temperature 4
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(MPA0, 1);
  digitalWrite(MPA1, 1);
  digitalWrite(LED, HIGH);
  foodC2 = readTemperature();
  delay(2000);
  digitalWrite(LED, LOW);
  */

  // LID SENSORS
    // Light Sensor
    byte msb = 0, lsb = 0;
    u_int16_t LTR_CH0_VALUE;
    u_int16_t LTR_CH1_VALUE;

    //channel 1
    Wire.beginTransmission(LTR_329_ADDR);
    Wire.write(LTR_CH1_LOW);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
    delay(1);
    if(Wire.available())
      lsb = Wire.read();
    
    Wire.beginTransmission(LTR_329_ADDR);
    Wire.write(LTR_CH1_HIGH);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
    delay(1);
    if(Wire.available())
      msb = Wire.read();

    LTR_CH1_VALUE = (msb<<8) | lsb;
    Serial.println(LTR_CH1_VALUE, DEC); //output in steps (16bit)

    //channel 0
    Wire.beginTransmission(LTR_329_ADDR);
    Wire.write(LTR_CH0_LOW);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
    delay(1);
    if(Wire.available())
      lsb = Wire.read();

    Wire.beginTransmission(LTR_329_ADDR);
    Wire.write(LTR_CH0_HIGH);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)LTR_329_ADDR, (uint8_t)1);
    delay(1);
    if(Wire.available())
      msb = Wire.read();

    LTR_CH0_VALUE = (msb<<8) | lsb;
    Serial.println(LTR_CH1_VALUE, DEC); //output in steps (16bit)

    // IMU


  // HOPPER SENSORS
    // Light Sensor
    // Not going to populate as I2C address is not selectable

    // ToF Sensor
    // needs development
  delay(1000);
}

float readTemperature(){
  v = spiRead();
  float thermocouple = 0;
  if (v == 0) {
    Serial.print("Temperature sensor not found\n");
    return thermocouple;
  }
  else {
    thermocouple = v * 0.25;
    Serial.println(thermocouple);
    return thermocouple;
  }
}

// IMU
  /*
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Begin with register 0x3B Accel_Xout_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14);
  int16_t t = Wire.read();

  AccX = (t << 8) | Wire.read();
  t = Wire.read();
  AccY = (t << 8) | Wire.read();
  t = Wire.read();
  AccZ = (t << 8) | Wire.read();
  t = Wire.read();
  Tmp = (t << 8) | Wire.read();
  t = Wire.read();
  GyroX = (t << 8) | Wire.read();
  t = Wire.read();
  GyroY = (t << 8) | Wire.read();
  t = Wire.read();
  GyroZ = (t << 8) | Wire.read();
  t = Wire.read();

  AccAngleX = (AccX) / 16384.0;
  AccAngleY = (AccY) / 16384.0;
  AccAngleZ = (AccZ)/ 16384.0;
  BoardTemp = (Tmp / 340) + 36.53;
  GyroAngleX = GyroX / 131.0;
  GyroAngleY = GyroY / 131.0;
  GyroAngleZ = GyroZ / 131.0;
  
  // Printing to Serial Monitor
  Serial.print(BoardTemp);
  Serial.print(" / ");
  Serial.print(AccAngleX);
  Serial.print(" / ");
  Serial.print(AccAngleY);
  Serial.print(" / ");
  Serial.print(AccAngleZ);
  Serial.print(" / ");
  Serial.print(GyroX);
  Serial.print(" / ");
  Serial.print(GyroY);
  Serial.print(" / ");
  Serial.println(GyroZ);
  */  
  /*
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU);
  Wire.write(0x1B);
  Wire.write(0);
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU);
  Wire.write(0x1C);
  Wire.write(0);
  Wire.endTransmission(true);
  */

// BLE
  /*
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
}
*/
  /*
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

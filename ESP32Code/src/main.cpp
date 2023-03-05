#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define PERIPHERAL_NAME "TestingBLE"
#define SERVICE_UUID "1a97c940-bb7b-11ed-a901-0800200c9a66" // Randomly generated UUID
#define LED 2

// ambient temperature reading
#define cs 5 // cs
#define clk 18 // sck
#define miso 19 // so
int v = 0;
float ambientC; // measured ambient temperature in Celcius

// Photoresistor measurement
#define PRLED 16
#define PRMEASURE 15
int prValue;

// IMU reading
const int MPU = 0x68;
int16_t AccX, AccY, AccZ, Tmp, GyroX, GyroY, GyroZ;
int16_t AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;
float AccAngleX, AccAngleY, AccAngleZ, GyroAngleX, GyroAngleY, GyroAngleZ, BoardTemp;
float ElapsedTime, CurrentTime, PreviousTime;

// SPI reading for temperature
int spiRead() {
  int rawtmp = 0;
  digitalWrite(cs, LOW);
  delay(2);
  digitalWrite(cs, HIGH);
  delay(200);

  // bit 15
  digitalWrite(cs,LOW); // Start conversation
  digitalWrite(clk, HIGH);
  delay(1);
  digitalWrite(clk,LOW); // bit read on down

  // bit 14 - 0
  for (int i = 14; i > 0; i--){
    digitalWrite(clk,HIGH);
    rawtmp += digitalRead(miso) << i;
    digitalWrite(clk,LOW);
  }

  if ((rawtmp & 0x04) == 0x04) return -1;
  return rawtmp >> 3;
}


// Setup
void setup() {
BLEDevice::init(PERIPHERAL_NAME);
BLEServer *pServer = BLEDevice::createServer();
BLEService *pServer = pServer->createService(SERVICE_UUID);



  pinMode (LED, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  // ambient temperature
  /*
  pinMode(cs, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(miso,INPUT);
  digitalWrite(cs, HIGH);
  digitalWrite(clk, LOW);
  */

  // IMU
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

  // Photoresistor
  pinMode(PRLED,OUTPUT);
  pinMode(PRMEASURE,INPUT);

}

// Loop indefinitely 
void loop() {
  // LED On to signal new reading
  //digitalWrite(LED, HIGH);
  //delay(250);

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

  // Ambient temperature reading
  /*
  v = spiRead();
  if (v == -1) {
    Serial.print("Temperature sensor not found");
  }
  else {
    ambientC = v * 0.25;
    Serial.println(ambientC);
  }
  */

  // Photoresistor
  prValue = analogRead(PRMEASURE);
  if (prValue > 100) {
    digitalWrite(LED,HIGH);
    Serial.print(prValue);
    Serial.println("greater");
  }
  else{
    digitalWrite(LED,LOW);
    Serial.print(prValue);
    Serial.println("Lessthan");
  }
  delay(250);
  //digitalWrite(LED, LOW);
  delay(250);
  
}



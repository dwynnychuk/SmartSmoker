#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#define LED 2

#define cs D8; // cs
#define clk D5 // sck
#define miso D6 // so
int v = 0;
float ambientC; // measured ambient temperature in Celcius

const int MPU = 0x68;
int16_t AccX, AccY, AccZ, Tmp, GyroX, GyroY, GyroZ;
int16_t AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;
float AccAngleX, AccAngleY, AccAngleZ, GyroAngleX, GyroAngleY, GyroAngleZ, BoardTemp;
float ElapsedTime, CurrentTime, PreviousTime;

void setup() {
  pinMode (LED, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

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

}


void loop() {
  // LED On to signal new reading
  digitalWrite(LED, HIGH);
  delay(200);

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

  digitalWrite(LED, LOW);
  delay(100);
  
}


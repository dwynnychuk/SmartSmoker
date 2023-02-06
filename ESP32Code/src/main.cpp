#include <Arduino.h>
#include <Wire.h>
#define LED 2

const int MPU = 0x68;
int16_t AccX, AccY, AccZ, Tmp;
int16_t GyroX, GyroY, GyroZ;
int16_t AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroAngleZ;
float roll, pitch, yaw;
int16_t AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float ElapsedTime, CurrentTime, PreviousTime;
int c = 0;


void calculate_IMU_Error() {
  // specified number of static readings to determine offset
  int num_error = 500;
  Serial.println(num_error);
}


void setup() {
  pinMode (LED, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  calculate_IMU_Error();
  delay(2000);
  }


void loop() {
  // LED On to signal new reading
  digitalWrite(LED, HIGH);
  delay(100);

  
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
  
  // Printing to Serial Monitor
  Serial.print(Tmp);
  Serial.print(" / ");
  Serial.print(AccX);
  Serial.print(" / ");
  Serial.print(AccY);
  Serial.print(" / ");
  Serial.println(AccZ);
  
  

  digitalWrite(LED, LOW);
  delay(10);
  
}


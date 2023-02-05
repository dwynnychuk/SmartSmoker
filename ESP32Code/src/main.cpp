#include <Arduino.h>
#include <Wire.h>

#define LED 2

const int MPU = 0x68;
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float ElapsedTime, CurrentTime, PreviousTime;
int c = 0;

void calculate_IMU_Error() {
  // specified number of static readings to determine offset
  int num_error = 500;
  while(c < num_error) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,6,true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    // Sum readings

    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ),2))) * 180/PI));
    AccErrorY = AccErrorY + ((atan(-1*(AccX)/sqrt(pow((AccY),2)+ pow((AccZ),2)))*180/PI));
  }

  AccErrorX = AccErrorX / num_error;
  AccErrorY = AccErrorY / num_error;
  c = 0;

  // Gyro Error

  while(c < num_error) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,6,true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum Readings

    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
  }

  GyroErrorX = GyroErrorX / num_error;
  GyroErrorY = GyroErrorY / num_error;
  GyroErrorZ = GyroErrorZ / num_error;

  // Printing to Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}

void setup() {
  pinMode (LED, OUTPUT);
  Serial.begin(115200);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  calculate_IMU_Error();
  delay(20);
  }


void loop() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(5000);
}
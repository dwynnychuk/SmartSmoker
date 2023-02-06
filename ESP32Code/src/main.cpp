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
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,6);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    // Sum readings

    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ),2))) * 180/PI));
    AccErrorY = AccErrorY + ((atan(-1*(AccX)/sqrt(pow((AccY),2)+ pow((AccZ),2)))*180/PI));
    c++;
  }

  AccErrorX = AccErrorX / num_error;
  AccErrorY = AccErrorY / num_error;
  c = 0;

  // Gyro Error

  while(c < num_error) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,6);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum Readings

    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
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
  Serial.print("begin");
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  calculate_IMU_Error();
  delay(2000);
  }


void loop() {
  // LED On to signal new reading
  digitalWrite(LED, HIGH);
  delay(100);

  
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  AccAngleX = (atan(AccY/sqrt(pow(AccX,2) + pow(AccZ,2))) * 180/PI) - AccErrorX;
  AccAngleY = (atan(-1*AccX/sqrt(pow(AccY,2) + pow(AccZ,2))) * 180/PI) - AccErrorY;

  PreviousTime = CurrentTime;
  CurrentTime = millis();
  ElapsedTime = (CurrentTime - PreviousTime)/1000;

  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6);

  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;

  GyroX = GyroX - GyroErrorX;
  GyroY = GyroY - GyroErrorY;
  GyroZ = GyroZ - GyroErrorZ;

  GyroAngleX = GyroAngleX + GyroX * ElapsedTime;
  GyroAngleY = GyroAngleY + GyroY * ElapsedTime;
  yaw = yaw + GyroZ * ElapsedTime;

  // Comlplementary Filter
  float gyroPercent = 0.96;
  roll = gyroPercent*GyroAngleX + (1-gyroPercent)*AccAngleX;
  pitch = gyroPercent*GyroAngleY + (1-gyroPercent)*AccAngleY;

  // Printing to Serial Monitor
  Serial.print(roll);
  Serial.print(" / ");
  Serial.print(pitch);
  Serial.print(" / ");
  Serial.println(yaw);
  
  

  digitalWrite(LED, LOW);
  delay(10);
  
}


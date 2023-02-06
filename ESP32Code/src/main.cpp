#include <Arduino.h>
#include <Wire.h>
#define LED 2

const int MPU = 0x68;
int16_t AccX, AccY, AccZ, Tmp, GyroX, GyroY, GyroZ;
int16_t AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;
float AccAngleX, AccAngleY, AccAngleZ, GyroAngleX, GyroAngleY, GyroAngleZ, BoardTemp;
float ElapsedTime, CurrentTime, PreviousTime;

void calculate_IMU_Error() {
  // specified number of static readings to determine offset
  int c = 0;
  int num_error = 500;
  Serial.begin(115200);
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Begin with register 0x3B Accel_Xout_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14);
  int16_t tn = Wire.read();

  while (c < num_error) {
    AccX = (tn << 8) | Wire.read();
    tn = Wire.read();
    AccY = (tn << 8) | Wire.read();
    tn = Wire.read();
    AccZ = (tn << 8) | Wire.read();
    tn = Wire.read();
    Tmp = (tn << 8) | Wire.read();
    tn = Wire.read();
    GyroX = (tn << 8) | Wire.read();
    tn = Wire.read();
    GyroY = (tn << 8) | Wire.read();
    tn = Wire.read();
    GyroZ = (tn << 8) | Wire.read();
    tn = Wire.read();

    // Sum values
    AccErrorX = AccErrorX + AccX;
    AccErrorY = AccErrorY + AccY;
    AccErrorZ = AccErrorZ + AccZ;
    GyroErrorX = GyroErrorX + GyroX;
    GyroErrorY = GyroErrorY + GyroY;
    GyroErrorZ = GyroErrorZ + GyroZ;
    Serial.println(c);
    c++;
  }
  // Divide
  AccErrorX = AccErrorX / num_error;
  AccErrorY = AccErrorY / num_error;
  AccErrorZ = AccErrorZ / num_error;
  GyroErrorX = GyroErrorX / num_error;
  GyroErrorY = GyroErrorY / num_error;
  GyroErrorZ = GyroErrorZ / num_error;

  Serial.println("FIND THE ERROR");
}


void setup() {
  pinMode (LED, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

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

  calculate_IMU_Error();
  delay(4000);
  Serial.print("Hello");
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

  AccAngleX = (AccX - AccErrorX) / 16384.0;
  AccAngleY = (AccY - AccErrorY) / 16384.0;
  AccAngleZ = (AccZ - AccErrorZ)/ 16384.0;
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
  

  digitalWrite(LED, LOW);
  delay(10);
  
}


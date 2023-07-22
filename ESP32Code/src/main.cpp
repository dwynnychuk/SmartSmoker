#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define LED 17 // Internal LED

#define MPA0 2 // A0 for Multiplexer
#define MPA1 15 // A1 for Multiplexer

#define cs 5 // SPI CS
#define clk 18 // SPI SCK
#define miso 19 // SPI MISO


const int MPU = 0x68; // IMU
int16_t AccX, AccY, AccZ, Tmp, GyroX, GyroY, GyroZ;
int16_t AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;
float AccAngleX, AccAngleY, AccAngleZ, GyroAngleX, GyroAngleY, GyroAngleZ, BoardTemp;
float ElapsedTime, CurrentTime, PreviousTime;
int v = 0;
float ambientC, internalC, foodC1, foodC2; // measured temperatures in celcius


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

/*
// BLE
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

void readTemperature();
*/

// Setup
void setup() {
  pinMode(LED, OUTPUT);
}

//  Serial.begin(115200);
//  Wire.begin();

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
}
*/

// Loop indefinitely 
void loop() {
  // LED On to signal new reading
  // Temperature 1
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(MPA0, 0);
  digitalWrite(MPA1, 0);
  digitalWrite(LED, HIGH);
  delay(1000);
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
  delay(1000);
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
  delay(1000);
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
  delay(1000);
  digitalWrite(LED, LOW);

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
  readTemperature();
  Serial.println(ambientC);
  // BLE
  if (deviceConnected){

    delay(500);
  }
  */

  // Photoresistor
  /*
  prValue = analogRead(PRMEASURE);
  if (prValue > 100) {
    digitalWrite(LED,HIGH);
    Serial.print(prValue);
    Serial.println(" greater");
  }
  else{
    digitalWrite(LED,LOW);
    Serial.print(prValue);
    Serial.println(" Lessthan");
  }
  delay(250);
  //digitalWrite(LED, LOW);
  delay(250);
  */
  
}


/*
void readTemperature(){
  v = spiRead();
  if (v == -1) {
    Serial.print("Temperature sensor not found");
  }
  else {
    ambientC = v * 0.25;
  }
}
*/

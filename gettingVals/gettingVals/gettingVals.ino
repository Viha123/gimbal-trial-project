#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;
//the device will come up in sleep mode upon power up
//gyro is in degrees per seconds
// so in order to get degrees, I must multiply by seconds passed 
//since there is an individual rotating the gyro, I don't need to have super high rpm
//this allows me to choose a 
int mpu = 0b1101000;
float accelConverter = 16384.0;
float gyroConverter = 131.0;
void setup(){
  Serial.begin(9600);

  Wire.begin();
  setUpMpu();
}
void loop(){
  //nothing for now
  // setUpMpu();
  recordAccelRegisters();
  recordGyroRegisters();
  printData();
  delay(100);
}

void setUpMpu(){ //in order to establish communication with the mpu, and set up all the registers 
  Wire.beginTransmission(0b1101000); //I2C address of the mpu (pg 33 of datasheet pdf)
  Wire.write(0x6B); //accessing register 6B - Power management () - basically selecting the register I want to write to
  Wire.write(0b00000000); //Setting sleep register to 0
  Wire.endTransmission();
  //setting up gyro
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1B);//specifying the register we are talking to (in this case the gyro) (Register sheet: 4.4)
  Wire.write(0x00000000); // setting gyro to fill scall +-250 degress
  Wire.endTransmission();
  //set up accelerometer
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1C);
  Wire.write(0b00000000); //setting accelerometer to +- 2g
  Wire.endTransmission();
}
void recordAccelRegisters(){
  Wire.beginTransmission(0b1101000);
  Wire.write(0x3B); //Data sheet (4.17) LSB-sensitivity: 16384LSB/g
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); //getting 6 registers from registers 
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read();
  accelY = Wire.read()<<8|Wire.read();
  accelZ = Wire.read()<<8|Wire.read();
  
  translateAccelData();
}
void translateAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0;
  gForceZ = accelZ / 16384.0;
}
void recordGyroRegisters(){
  Wire.beginTransmission(0b1101000);
  Wire.write(0x43); //register map thingie 4.19
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6);
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read();
  gyroY = Wire.read()<<8|Wire.read();
  gyroZ = Wire.read()<<8|Wire.read();
  
  translateGyroData();
}
void translateGyroData(){
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0;
  rotZ = gyroZ / 131.0;
}
void printData(){
  Serial.print("Gyro (deg) :");
  Serial.print("X= :");
  Serial.print(rotX);
  Serial.print("Y= :");
  Serial.print(rotY);
  Serial.print("Z= :");
  Serial.print(rotZ);
  Serial.print("Accel (g) :");
  Serial.print("X= :");
  Serial.print(gForceX);
  Serial.print("Y= :");
  Serial.print(gForceY);
  Serial.print("Z= :");
  Serial.println(gForceZ);
  Serial.println("");
}
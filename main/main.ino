#include <Servo.h> //in order to use the servo
#include <Wire.h> //in order to communicate with my MPU 6050
Servo myServo; //initializing servo of type Servo
#define ADDRESS 104;
/*
Notes about I2C Bus Protocol (cuz i have no idea):
-according to internet: address of my mpu6050:0x68 (hexadecimal)
-apparently: hexadecimal to decimal is 104!
*/
/*General facts about the sensor
-mpu gives out 3 accelerometer values and 3 gyro vals
- pretty sure I need the gyro vals
-might require more addresses for the gyro sensor and accelerometer sensor
-need internal register address for gyro x, y and z axis
- random thought : do we need to use some sort of pid for this?
*/
int servoPin = 9;
int servoPos = 110; //need to test this so its on the top at the beginning of demonstration
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;
int mpuAdd = 0x68;
float prevZ; 
float deltaZ;
float gyroConverter = 131.0;
float sum = 0;
float turnVal = 0;
int delayTime = 300;
double initialSeconds, finalSeconds, duration;
float temp;
int counter;
float gyroErrorX, gyroErrorY, gyroErrorZ;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600); //setting up serial monitor
  setUpMpu();
  myServo.attach(servoPin);
  myServo.write(servoPos);
  // Serial.println("CAlculating gyro error");
  // calculateGyroError();
  // Serial.println("Finished Calculating gyro error");
}

void loop() {
  // put your main code here, to run repeatedly:
  // servoMove(100,30,true);
  counter ++;
  initialSeconds = finalSeconds; //gets initialSeconds
  // prevZ = rotZ;  //not required anymore due to different math being used


  finalSeconds = millis();
  duration = (finalSeconds - initialSeconds) / 1000 ; // this will be in milliseconds so divide by 1000 to get millisecods
  Serial.println(duration);
  recordGyroRegisters();

  // rotZ = rotZ * duration; //this only works for constant angular velocity 
  //angular displacement (theta) = integral of angular velocity from initialSeconds to finalSeconds
  // if(counter % 20 == 0){
  //   temp = sum;
  // }
  temp = sum;
  
  // deltaZ = rotZ - prevZ;
  // Serial.println(temp);
   //converting duration to seconds and then multipying to get rotation. //kinda like the rectangle under the curve
  // Serial.println(rotZ);
  // if(abs(rotZ) > 4){ //eliminating some kind of noise
  //   sum += rotZ; //summing all the small rectangles in the curve
  // }
  rotZ = rotZ * duration;
  sum += rotZ;
  turnVal = sum - temp; 
  // if(counter % 100 == 0){
    // Serial.print("Sum : ");
    // Serial.print(sum);
    // Serial.print("Turn Val");
    // Serial.println(turnVal);
  
  // Serial.println(sum);
  turnVal = floor(turnVal);
  if(turnVal < -2){ // sensor turns right, make servo turn that many degress left ->check this function again
    servoMove(servoPos, -turnVal, false); //initially this was -
  }
  if(rotZ > 2){ // sensor turns right, make servo turn that many degress left 
    servoMove(servoPos, turnVal, true); // this was +
  }
  // Serial.print("SumZ (degrees) ");
  // Serial.println(sum);
  printData(); //test it with a sure 90
  // Serial.print("turn val "); //theoretically the amount of degrees moved
  // Serial.println(turnVal);
  if(servoPos > 180){
    servoPos = 180;
    Serial.println("Servo max limit reached");
    myServo.write(servoPos);
  }
  if(servoPos < 0){
    servoPos = 0;
    Serial.println("Servo min limit reached");
    myServo.write(servoPos);
  }
  //might need to account for sensor drift
  delay(delayTime);

}

void setUpMpu(){ //in order to establish communication with the mpu, and set up all the registers 
  Wire.beginTransmission(mpuAdd); //I2C address of the mpu (pg 33 of datasheet pdf)
  Wire.write(0x6B); //accessing register 6B - Power management () - basically selecting the register I want to write to
  Wire.write(0b00000000); //Setting sleep register to 0
  Wire.endTransmission();
  //setting up gyro
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x1B);//specifying the register we are talking to (in this case the gyro) (Register sheet: 4.4)
  Wire.write(0x00000000); // setting gyro to fill scall +-250 degress
  Wire.endTransmission();
  //set up accelerometer
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x1C);
  Wire.write(0b00000000); //setting accelerometer to +- 2g
  Wire.endTransmission();
}
void recordGyroRegisters(){
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x43); //register map thingie 4.19
  Wire.endTransmission();
  Wire.requestFrom(mpuAdd, 6);
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read();
  gyroY = Wire.read()<<8|Wire.read();
  gyroZ = Wire.read()<<8|Wire.read();
  
  translateGyroData();
}
void translateGyroData(){ 
  rotY = gyroY / gyroConverter + 4.87;
  rotX = gyroX / gyroConverter + 2.35;
  rotZ = gyroZ / gyroConverter + 0.82; //gyro error was -0.88


}
void printData(){ //these values will be in degrees per second. Thats why they end up going back to their original values
  //perhaps the error is what the values are when they are 
  Serial.print("Gyro (deg) :");
  Serial.print("X= :");
  Serial.print(rotX);
  Serial.print("Y= :");
  Serial.print(rotY);
  Serial.print("Z= :");
  Serial.print(rotZ);
  // Serial.print("turn val= ");
  // Serial.println(turnVal);
  // Serial.print("  ");
  // Serial.print("Change in z: ");
  // Serial.println(deltaZ);
  //ONLY WHEN GRAPH VALUES ARE NEGATIVE DOES THE THING GO DOWN
}
void calculateGyroError(){ //one time function that needs to be called at the start to figure out error
  int c = 0;
  while(c < 200){
    c++;
    Wire.beginTransmission(mpuAdd);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(mpuAdd, 6, true);
    // while(Wire.available())
    gyroX = Wire.read()<<8|Wire.read();
    gyroY = Wire.read()<<8|Wire.read();
    gyroZ = Wire.read()<<8|Wire.read();
    gyroErrorX += (gyroX/131.0);
    gyroErrorY += (gyroY/131.0);
    gyroErrorZ += (gyroZ/131.0);
  }
  gyroErrorX = gyroErrorX / 200;
  gyroErrorY = gyroErrorY / 200;
  gyroErrorZ = gyroErrorZ / 200;
  Serial.print("Gyro error X ");
  Serial.println(gyroErrorX);
  Serial.print("Gyro error Y ");
  Serial.println(gyroErrorY);
  Serial.print("Gyro error z ");
  Serial.println(gyroErrorZ); //i only care about gyroerrorz;

}
void servoMove(int currentPos, int degrees, bool direction){ // if direction = true, turn right, if direction = false turn left. Possible helper function
  if(direction){//going right
    int finalX = currentPos - degrees;
    for(int i = currentPos; i > finalX; i --){ //going right
      myServo.write(i);
      delay(5);
    }
    servoPos = finalX;

  }
  else{ //going left a certain amoutn of degress
    int finalX = currentPos + degrees;
    for(int i = currentPos; i < finalX; i ++){ //going right
      myServo.write(i);
      delay(5);
    }
    servoPos = finalX;

  }
}
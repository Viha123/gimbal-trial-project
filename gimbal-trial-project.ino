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
float gx, gy, gz; 

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600); //setting up serial monitor
  Wire.beginTransmission(ADDRESS); //begin talking to the mpu whose address is 
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  myServo.attach(servoPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  // servoMove(100,30,true);
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x43);//gryo data first register address
  Wire.endTransmission(false);
  Wire.requestFrom(ADDRESS, 6, true) //7bit slave address that we are talking to, 6 is the amount of bytes of info we are getting, true automatically closes the signal after data received
 
  // Wire.requestFrom(ADDRESS, 6, );
  // if(Wire.available()<=2){

  // }
}
void servoMove(int currentPos, int degrees, bool direction){ // if direction = true, turn right, if direction = false turn left. Possible helper function
  if(direction){//going right
    int finalX = currentPos - degrees;
    for(int i = currentPos; i > finalX; i --){ //going right
      myServo.write(i);
      delay(15);
    }
  }
  else{ //going left a certain amoutn of degress
    int finalX = currentPos + degrees;
    for(int i = currentPos; i < finalX; i ++){ //going right
      myServo.write(i);
      delay(15);
    }
  }
}
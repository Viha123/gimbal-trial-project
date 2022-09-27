#include <Servo.h>
Servo myServo; //initializing servo of type Servo

int servoPin = 9;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //setting up serial monitor
  myServo.attach(servoPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  servoMove(100,30,true);
}
void servoMove(int currentPos, int degrees, bool direction){ // if direction = true, turn right, if direction = false turn left
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
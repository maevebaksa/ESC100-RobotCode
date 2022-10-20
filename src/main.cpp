#include <Arduino.h>
#include <Servo.h>

// SET THE PINS FOR YOUR SPECIFIC CONFIGURATION HERE:

//button pins
const int rightButton = 7;
const int leftButton = 8;

//servo PWM pins
const int leftServoPWM = 6;
const int rightServoPWM = 3;
const int centerServoPWM = 5;

//right motor pins
const int rightMotorPin1 = 12;
const int rightMotorPin2 = 13;
const int rightMotorPWM = 11;

//left motor pins
const int leftMotorPin1 = 4;
const int leftMotorPin2 = 2;
const int leftMotorPWM = 9;

//current sense pins
const int rightServoSense = A1;
const int leftServoSense = A0;

//CALIBRATION: 

//calibrate the motor stop threshold
//set this value to "true" then 
//check the serial monitor for the value
//that is printed once the motor
//is touching the book page, then subract 20
//set this value in the threshold value.
bool calibrate = false;
int threshold = 0;

// continuous rotation servos must 
//be adjusted in a way so that at 1500us 
// width they will not spin, by
//enabling this setting it will allow
//for you to adjust the potentiometer to
//stop the motor from drifting.
bool servoStopCalibrate = false;

//TIMING / MOTORS:
//adjust for how long the center servo is to spin in order to make a full rotation
//additionally, adjust the speed of the servos

int spinningTime = 0;
int uSforClockwise = 800;
int uSforCounterClockwise = 2200;

//how long to wait for the current to stabilize 
//and to wait before moving the servo further
int delayBetweenDegrees = 10;

//how far the motor should turn backwards
//in degrees in order to not apply too much 
//pressure on the pages
int backoffDegrees = 1;

//set these for the home positions
//of the individual servo motors
int rightServoHome = 0;
int leftServoHome = 180;

//use this to set how far the servo should go max
//this should not be greater than 180
int maxAngle = 180;

//use these to set the direction of the wheel motor
int leftMotorOneValue = 1;
int leftMotorTwoValue = 1;
int rightMotorOneValue = 1;
int rightMotorTwoValue = 1;

int motorSpeed = 255;

//DO NOT ADJUST FROM THIS POINT, PRE-SETUP CODE:

//create servo objects
Servo leftServo;
Servo rightServo;
Servo centerServo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //set the pinmodes.
  pinMode(rightButton,INPUT_PULLUP);
  pinMode(leftButton,INPUT_PULLUP);
  pinMode(leftServoPWM,OUTPUT);
  pinMode(rightServoPWM,OUTPUT);
  pinMode(centerServoPWM,OUTPUT);
  pinMode(rightMotorPin1,OUTPUT);
  pinMode(rightMotorPin2,OUTPUT);
  pinMode(rightMotorPWM,OUTPUT);
  pinMode(leftMotorPin1,OUTPUT);
  pinMode(leftMotorPin2,OUTPUT);
  pinMode(leftMotorPWM,OUTPUT);

  //properly attach pins to servos

  rightServo.attach(rightServoPWM);
  leftServo.attach(leftServoPWM);
  centerServo.attach(centerServoPWM);

  //move the servos back to top
  rightServo.write(0);
  leftServo.write(0);
  centerServo.write(0);
}

//the main function 
void turnPage (bool direction, bool wheelsDown){
  int position = 0;
  int motorPin1;
  int motorPin2;
  int motorPWM;
  int uSforCenter;
  int sensePin;
  int otherSensePin;
  int pin1Value;
  int pin2Value;

  //based on the motor direction, set the 
  //pins and values so that everything turns
  //correctly
  //I know this is awful and inneficient, I
  //tried making it better but this will have to do.
  if (direction == 0){
    motorPin1 = leftMotorPin1;
    motorPin2 = leftMotorPin2;
    motorPWM = leftMotorPWM;
    uSforCenter = uSforCounterClockwise;
    sensePin = leftServoSense;
    otherSensePin = rightServoSense;
    pin1Value = leftMotorOneValue;
    pin2Value = leftMotorTwoValue;
  }

  else if (direction == 1){
    motorPin1 = rightMotorPin1;
    motorPin2 = rightMotorPin2;
    motorPWM = rightMotorPWM;
    uSforCenter = uSforClockwise;
    sensePin = rightServoSense;
    otherSensePin = leftServoSense;
    pin1Value = rightMotorOneValue;
    pin2Value = rightMotorTwoValue;
  } 

  //move all wheels to start position
  rightServo.write(rightServoHome);
  leftServo.write(leftServoHome);
  centerServo.write(0);

  //slowly increase the angle of the servo until it reaches the
  //stalling threshold
  while(analogRead(sensePin) < threshold && position < maxAngle){
    position ++;
    if (direction == 0){
      leftServo.write(position);
    }
    if (direction == 1){
      rightServo.write(position);
    }
    delay(delayBetweenDegrees);
  }

  //at this point, back off the servos by the 
  //back off degrees value
  if (direction == 0){
    leftServo.write(position-backoffDegrees);
  }
  if (direction == 1){
    rightServo.write(position-backoffDegrees);
  }

  //now it is time to spin the wheel, everything
  //here is set by variable so it should be really easy

  digitalWrite(motorPin1,pin1Value);
  digitalWrite(motorPin2,pin2Value);
  analogWrite(motorPWM,motorSpeed);

  //now it is time to finally flip the page using the center arm
  
  centerServo.writeMicroseconds(uSforCenter);
  delay(spinningTime);
  centerServo.writeMicroseconds(1500);

  //now check if the wheels should grip the page as per
  //the initial call for this function, if the wheels down
  //variable is one, then this code should lower the wheels onto
  //the page.

  if (wheelsDown == 1){
    // lower the other motor onto the page
    while(analogRead(otherSensePin) < threshold && position < maxAngle){
      position ++;
      if (direction == 0){
        rightServo.write(position);
      }
      if (direction == 1){
        leftServo.write(position);
      }
      delay(delayBetweenDegrees);
    }
  }
  //if the wheels aren't supposed to remain on the page, 
  //send them back to the home
  else if(wheelsDown == 0){
    leftServo.write(leftServoHome);
    rightServo.write(rightServoHome);
  }

}


void loop() {
  // put your main code here, to run repeatedly:
  if (calibrate){
    int position = 0;

    while(position < maxAngle){
      position ++;
      rightServo.write(position);
      leftServo.write(position);

      Serial.print("right: ");
      Serial.println(analogRead(rightServoSense));
      Serial.print("left: ");
      Serial.println(analogRead(leftServoSense));
    }
  }
  else if (servoStopCalibrate){

    //move the right and left servo to
    //the proper home position to 
    //ensure it is in the right place

    rightServo.write(rightServoHome);
    leftServo.write(leftServoHome);

    //make the center servo stop and wait in the
    //center so you can adjust the pot
    centerServo.writeMicroseconds(1500);

    //wait 100 seconds for the user to
    //complete calibration
    delay(100000);
  } 

  else{
    //main void loop code:
    
    //checks if left button is pressed, and correspondingly calls the
    //"turn page" function
    //bool direction turns page backwards if val of 0
    //bool direction turns page forward if val of 1
    //bool wheelsDown leaves the wheels in a downward position if there's a value of 1

    if (digitalRead(leftButton) == LOW){
      turnPage(0,1);
    }
    if (digitalRead(rightButton) == LOW){
      turnPage(1,1);
    }
  }

}
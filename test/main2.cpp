#include <Arduino.h>
#include <Servo.h>

//servo PWM pins
const int leftServoPWM = 6;
const int rightServoPWM = 3;
const int centerServoPWM = 5;

Servo rightservo;
Servo leftservo;
Servo centerservo;

void setup(){

  Serial.begin(9600);
  rightservo.attach(rightServoPWM);
  leftservo.attach(leftServoPWM);
  centerservo.attach(centerServoPWM);

}

void loop(){

  delay(1000);

  rightservo.write(90);
  leftservo.write(90);
  centerservo.write(90);


}
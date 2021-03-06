#include <XBOXRECV.h>
#include <Servo.h>
#include "pitches.h"

//need a pin for emergency stop button(s)

//Actuator variables
const int actrCtrlPin = 2;              //Pin to control actuator movement
const int hallSensorPin = A0;           //Pin to read Hall Effect Sensor

Servo actuator;                         //Servo output for actuator controller

const int limitTop = 100;               //Value from Hall Effect Sensor for max extension
const int limitBot = 0;                 //Value from Hall Effect Sensor for max retraction
const int actrUp = 100;                 //Value used to extend actuator
const int actrDown = 100;               //Value used to retract actuator
//const int actrHold = 0;                 //Value used to hold actuator in position
bool sensorOK = true;                   //Boolean value checked when left stick is moved
bool emergencyStop = false;             //Boolean value, prevents all inputs while true, must reset arduino to continue use

//Motor variables
const int motorSpeed1Pin = 8;           //Pin to control speed of motor 1
const int motorSpeed2Pin = 9;           //Pin to control speed of motor 2

Servo motor1;                           //Servo output for motor controller 1
Servo motor2;                           //Servo output for motor controller 2

const int reverseMax = 46;              //Value to send to motor controller for maximum reverse speed
const int reverseMin = 90;              //Value to send to motor controller for minimum reverse speed
const int stationary = 92;              //Value to send to motor controller for no speed
const int forwardMin = 94;              //Value to send to motor controller for minimum forward speed
const int forwardMax = 141;             //Value to send to motor controller for maximum forward speed

//Ultrasonic sensor variables
const int ultraSensorTX = 14;           //Pin for trig
const int ultraSensorRX = 15;           //Pin for echo
const int buzzardBackwards = 13;        //Pin to send signal to buzzer

//Controller variables
const int conThresh = 7500;             //Value to check joystick position against for activation (minimum press to move)
const int conLimit = 32767;             //Value for the limit of joystick position (needs verified)

//These outputs are all for testing purposes 
int forwardLED = 22; 
int leftLED = 23;
int rightLED = 24;
int upLED = 25; 
int downLED = 26; 
int stopLED = 27;
int backwardLED = 30;

int melody[] = {                         //select the notes you want to play
  NOTE_E2/*, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 */
};

int noteDurations[] = {                 //4= quarter note, 8 = either note
   1/*, 4, 4, 4 , 4, 4, 4, 4*/
};

USB Usb;
XBOXRECV Xbox(&Usb);

void setup() {
  Serial.begin(115200);
  Usb.Init(); 
  actuator.attach(actrCtrlPin);
  motor1.attach(motorSpeed1Pin);
  motor2.attach(motorSpeed2Pin);
  //pinMode(buzzardBackwards, INPUT); //beeping for backwards
  pinMode(ultraSensorRX, INPUT);
  pinMode(ultraSensorTX, OUTPUT);
  
  //Debug pins
  pinMode(forwardLED, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
  pinMode(upLED, OUTPUT);
  pinMode(downLED, OUTPUT);
  pinMode(stopLED, OUTPUT);
  pinMode(backwardLED, OUTPUT);
}

void loop() {
  if (!emergencyStop) {
    Usb.Task();
    if (Xbox.XboxReceiverConnected) {
      bool moving = false;
      if (sensorOK) {
        if (Xbox.getAnalogHat(LeftHatY, 0) > conThresh) {
          //going forward
          if (Xbox.getButtonClick(Y, 0)) {
            ForwardMovement(forwardMax);
          } else {
            ForwardMovement((forwardMin + ((forwardMax - forwardMin) / 4)), (reverseMin - ((reverseMin - reverseMax)/4)));
          }
          moving = true;
        } else if (Xbox.getAnalogHat(LeftHatY, 0) < -conThresh) {   
          //going backwards
          //buzzard
          if (Xbox.getButtonClick(Y, 0)) {
            BackwardMovement(reverseMax);
          } else {
            BackwardMovement((forwardMin + ((forwardMax - forwardMin) / 4)), (reverseMin - ((reverseMin - reverseMax)/4)));
          }
          moving = true;
        } else if (Xbox.getAnalogHat(LeftHatX, 0) > conThresh) {
          //going right???
          digitalWrite(rightLED, HIGH);
          moving = true;
        } else if (Xbox.getAnalogHat(LeftHatX, 0) < -conThresh) {   
          //going left???
          digitalWrite(leftLED, HIGH);
          moving = true;
        } else {
          NoMovement();
          digitalWrite(forwardLED, LOW);
          digitalWrite(leftLED, LOW);
          digitalWrite(rightLED, LOW);
          digitalWrite(backwardLED, LOW);
          moving = false;
        }
      } else {
        NoMovement();
        if (Xbox.getButtonClick(A, 0)) {
          // Button is pressed
         ButtonA_pressed();
        }
      }

      if (!moving) {
        int pos = analogRead(hallSensorPin);                      //Get actuator vertical position
        pos = 10;
        if (Xbox.getAnalogHat(RightHatY , 0) > conThresh) {       //Check if Right Stick is in up direction
          if (pos < limitTop) {                                   //Check if actuator can extend
            actuatorUp();
            digitalWrite(upLED, HIGH);
          } else {
            actuatorHold();
            digitalWrite(upLED, LOW);
            digitalWrite(downLED, LOW);
          }
        } else if (Xbox.getAnalogHat(RightHatY, 0) < -conThresh) {  //Check if Right Stick is in down direction
          if (pos > limitBot) {                                   //Check if actuator can retract
            actuatorDown();
            digitalWrite(downLED, HIGH);
          } else {
            actuatorHold();
            digitalWrite(upLED, LOW);
            digitalWrite(downLED, LOW);
          }
        } else {                                                    //If Right Stick is not moved
          actuatorHold();
          digitalWrite(upLED, LOW);
          digitalWrite(downLED, LOW);
        }
      } else {
        actuatorHold();
        digitalWrite(upLED, LOW);
        digitalWrite(downLED, LOW);
      }
      
     if (Xbox.getButtonClick(B, 0)) {
        // Button is pressed
        ButtonB_pressed();
      } else if (false) {
        //change false to pin emergency stop button is on
      }
      delay(1);
    }
  delay(1);
  } else {
    NoMovement();
    digitalWrite(stopLED, HIGH);
  }
}

void actuatorUp() {
  //int stick = Xbox.getAnalogHat(RightHatY, 0);
  //int movement = map(stick, conThresh, conLimit, forwardMin, forwardMax);
  actuator.write(forwardMax);          //or use actrUp to specify constant value if not max forward
  digitalWrite(upLED, HIGH);
}

void actuatorDown() {
  //int stick = Xbox.getAnalogHat(RightHatY, 0);
  //int movement = map(stick, -conThresh, -conLimit, reverseMin, reverseMax);
  actuator.write(reverseMax);          //or use actrDown to specify constant value if not max reverse
  digitalWrite(downLED, HIGH);
}

void actuatorHold() {
  actuator.write(stationary);
  digitalWrite(upLED, LOW);
  digitalWrite(downLED, LOW);
}



//Stephanie Stuff for movement
void ForwardMovement(int maxForward, int maxReverse) {
  long duration, distance;
  int stick = Xbox.getAnalogHat(LeftHatY, 0);
  
  //getting sensor to transmit??
  //will this give a delay?? We would need to make sure it is quick enough not to notice...
  digitalWrite(ultraSensorTX, LOW);
  delayMicroseconds(10);
  digitalWrite(ultraSensorTX, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraSensorTX, LOW);
  
  duration = pulseIn(ultraSensorRX, HIGH);
  distance = (duration/2) / 29.1;     //distance is in cm and is what value was returned
  
  if (distance > 25) {                //should give about 44 inches assuming 11 inch is 25
    sensorOK = true;
    if (distance >= 100) {
      //continue full speed ahead!
      int movement1 = map(stick, conThresh, conLimit, reverseMin, maxReverse);
      int movement2 = map(stick, conThresh, conLimit, forwardMin, maxForward);
      motor1.write(movement1);
      motor2.write(movement2);
      digitalWrite(forwardLED, HIGH);
    } else if (distance >= 75) {
      //continue at 2 thirds of full speed
      int max1 = forwardMin + (((maxForward - forwardMin)*2) / 3);
      int max2 = reverseMin - (((reverseMin - reverseMax)*2) / 3);
      int movement1 = map(stick, conThresh, conLimit, reverseMin, max2);
      int movement2 = map(stick, conThresh, conLimit, forwardMin, max1);
      motor1.write(movement1);
      motor2.write(movement2);
    } else if (distance >= 50) {     
      //continue at a third of full speed
      int max1 = forwardMin + ((maxForward - forwardMin) / 3);
      int max2 = reverseMin - ((reverseMin - reverseMax) / 3);
      int movement1 = map(stick, conThresh, conLimit, reverseMin, max2);
      int movement2 = map(stick, conThresh, conLimit, forwardMin, max1);
      motor1.write(movement1);
      motor2.write(movement2);
    } else { //25 should be about 11 inches away from it
      //continue a crawl of full speed
      int max1 = forwardMin + ((maxForward - forwardMin) / 6);
      int max2 = reverseMin - ((reverseMin - reverseMax) / 6);
      int movement1 = map(stick, conThresh, conLimit, reverseMin, max2);
      int movement2 = map(stick, conThresh, conLimit, forwardMin, max1);
      motor1.write(movement1);
      motor2.write(movement2);
    }
  } else {
    //Robot is too close, wait for confirmation to continue moving
    //Should give user feedback, check if blink middle LEDs is possible
    sensorOK = false;
    motor1.write(stationary);
    motor2.write(stationary);
  }
}

void BackwardMovement(int maxForward, int maxReverse) {
  //sensorOK = 1;
  //buzzardBackwards = 1; //figure out how to make it speak
  
  int stick = Xbox.getAnalogHat(LeftHatY, 0);
  int movement1 = map(stick, -conThresh, -conLimit, reverseMin, maxReverse);
  int movement2 = map(stick, -conThresh, -conLimit, forwardMin, maxForward);
  motor1.write(movement2);
  motor2.write(movement1);
  digitalWrite(backwardLED, HIGH);
  
  //for (int thisNote = 0; thisNote < 3; thisNote++) {
    //use this to calculate noteDuration: quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int thisNote = 0;
    int noteDuration = 10000 / noteDurations[thisNote];
    tone(buzzardBackwards, melody[thisNote], noteDuration);
    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    //int pauseBetweenNotes = noteDuration * 1.30;
    //delay(pauseBetweenNotes);
    //stop the tone playing:
    //tone(buzzardBackwards, melody[thisNote], noteDuration);
  noTone(13);
  //} 
}

void NoMovement() {
  motor1.write(stationary);
  motor2.write(stationary);
  actuator.write(stationary);
}

//Laura

void ButtonA_pressed() {
  //Allow robot to be moved after proximity detection
}

void ButtonB_pressed() {
  //Stop all current robot movement and prevent future movements
  emergencyStop = true;
  motor1.write(stationary);
  motor2.write(stationary);
  actuator.write(stationary);
}

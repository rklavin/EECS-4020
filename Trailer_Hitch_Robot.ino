
#include <XBOXRECV.h>
#include "pitches.h"                    //need to make this header file

//need a pin for emergency stop button(s)

const int actrCtrlPin = 2;              //Pin to control actuator movement
const int hallSensorPin = A0;           //Pin to read Hall Effect Sensor

const int limitTop = 100;               //Value from Hall Effect Sensor for max extension
const int limitBot = 0;                 //Value from Hall Effect Sensor for max retraction
const int actrUp = 100;                 //Value used to extend actuator
const int actrDown = 100;               //Value used to retract actuator
const int actrHold = 0;                 //Value used to hold actuator in position
bool sensorOK = true;
bool emergencyStop = false;

const int PIN_BUTTON_A = 20;
const int PIN_BUTTON_B = 21;

const int motorDir = 8; //Dir 1
const int motorSpeed = 9; //PWM 1
const int motorDir2 = 10; //Dir 2
const int motorSpeed2 = 11; //PWM2

const int ultraSensorOK = 12;
const int buzzardBackwards = 13;               //backwards buzzard


//these outputs are all for testing purposes 
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
  pinMode(actrCtrlPin, OUTPUT);
  pinMode(motorDir, OUTPUT); //Movement Part
  pinMode(motorDir2, OUTPUT); //Movement Part
  pinMode(motorSpeed, OUTPUT); //Movement Part
  pinMode(motorSpeed2, OUTPUT); //Movement Part
  //pinMode(ultraSensorOK, INPUT); //sensor okay to move?
  pinMode(buzzardBackwards, INPUT); //beeping for backwards
//pinMode(buzzer, OUTPUT); //buzzer
  pinMode(forwardLED, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
  pinMode(upLED, OUTPUT);
  pinMode(downLED, OUTPUT);
  pinMode(stopLED, OUTPUT);
  pinMode(backwardLED, OUTPUT);
  /*pinMode(PIN_BUTTON_A, INPUT);
  digitalWrite(PIN_BUTTON_A, LOW);  //question
  pinMode(PIN_BUTTON_B, INPUT);
  digitalWrite(PIN_BUTTON_B, LOW);  //question*/
}

void loop() {
  if (!emergencyStop) {
    Usb.Task();
    if (Xbox.XboxReceiverConnected) {
      bool moving = false;
      
      if (sensorOK) {
        if (Xbox.getAnalogHat(LeftHatY, 0) > 7500) {
          //going forward
          ForwardMovement();
          moving = true;
        } 
        else if (Xbox.getAnalogHat(LeftHatY, 0) < -10000) {   
          //going backwards
          //buzzard
          BackwardMovement();
          moving = true;
        }
        else if (Xbox.getAnalogHat(LeftHatX, 0) > 7500) {
          //going right???
          digitalWrite(rightLED, HIGH);
          moving = true;
        } 
        else if (Xbox.getAnalogHat(LeftHatX, 0) < -7500) {   
          //going left???
          digitalWrite(leftLED, HIGH);
          moving = true;
        }
        else {
          digitalWrite(forwardLED, LOW);
          digitalWrite(leftLED, LOW);
          digitalWrite(rightLED, LOW);
          digitalWrite(backwardLED, LOW);
          moving = false;
        }
      }
      else {
        if (Xbox.getButtonClick(A, 0)) {
          // Button is pressed
         ButtonA_pressed();
        }
      }

      if (!moving) {
        int pos = 10;//analogRead(hallSensorPin);                  //Get actuator vertical position
        if (Xbox.getAnalogHat(RightHatY , 0) > 7500) {        //Check if Right Stick is in up direction
          if (pos < limitTop) {                               //Check if actuator can extend
            actuatorUp();
            digitalWrite(upLED, HIGH);
          } else {
            actuatorHold();
            digitalWrite(upLED, LOW);
            digitalWrite(downLED, LOW);
          }
        } 
        else if (Xbox.getAnalogHat(RightHatY, 0) < -7500) {   //Check if Right Stick is in down direction
          if (pos > limitBot) {                               //Check if actuator can retract
            actuatorDown();
            digitalWrite(downLED, HIGH);
          } 
          else {
            actuatorHold();
            digitalWrite(upLED, LOW);
            digitalWrite(downLED, LOW);
          }
        } 
        else {                                              //If Right Stick is not moved
          actuatorHold();
          digitalWrite(upLED, LOW);
          digitalWrite(downLED, LOW);
        }
      }
      else {
        actuatorHold();
        digitalWrite(upLED, LOW);
        digitalWrite(downLED, LOW);
      }
      
     if (Xbox.getButtonClick(B, 0)) {
        // Button is pressed
        ButtonB_pressed();
      }
      else if (false) {
        //change false to pin emergency stop button is on
      }
      delay(1);
    }
  delay(1);
  }
  else {
    digitalWrite(stopLED, HIGH);
  }
}

void actuatorUp() {
  analogWrite(actrCtrlPin, actrUp);
  digitalWrite(upLED, HIGH);
}

void actuatorDown() {
  analogWrite(actrCtrlPin, actrDown);
  digitalWrite(downLED, HIGH);
}

void actuatorHold() {
  analogWrite(actrCtrlPin, actrHold);
  digitalWrite(upLED, LOW);
  digitalWrite(downLED, LOW);
}



//Stephanie Stuff for movement
void ForwardMovement() {
  int ultraSensor = analogRead(ultraSensorOK);
  if (ultraSensor > 5) {
    sensorOK = true;
    if (ultraSensor >= 10) {
      //continue full speed ahead!
      digitalWrite(forwardLED, HIGH);
    }
    else if (ultraSensor >= 8) {
      //continue a third of the speed
    }
    else if (ultraSensor >= 6) {
      //continue 2 thirds of full speed
    }
    else {
      //continue a crawl of full speed
    }
  }
  else {
    sensorOK = false;
    analogWrite(motorSpeed, 0);
    analogWrite(motorSpeed2, 0);
    //add in confirmation button
    //add in back movement is only allowed
    //Laura's function of button A needs to be called
    /*if (PIN_BUTTON_A = LOW) {
      ButtonA_pressed();
    }
    else {
      BackwardMovement();   //only allowed
    }*/
  }
}

void BackwardMovement() {
  //sensorOK = 1;
 // buzzardBackwards = 1; //figure out how to make it speak
  analogWrite(motorSpeed, 1); //motors same speed but slow
  analogWrite(motorSpeed2, 1); //motors same speed but slow
  digitalWrite(backwardLED, HIGH);
 // for (int thisNote = 0; thisNote < 3; thisNote++) {
    //use this to calculate noteDuration: quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int thisNote = 0;
    int noteDuration = 10000 / noteDurations[thisNote];
    tone(buzzardBackwards, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    //int pauseBetweenNotes = noteDuration * 1.30;
   // delay(pauseBetweenNotes);
    // stop the tone playing:
   //tone(buzzardBackwards, melody[thisNote], noteDuration);
  noTone(13);
 // } 
}

//Laura

void ButtonA_pressed() {
  //Maybe need some sort of loop to slowly move robot into position
  
  /*if (digitalRead(PIN_BUTTON_A) == LOW) {                        // Button A is pressed
    //both motors move forward at same speed at certain time();
  }
  else {
    //both motors move backward at same speed at certain time();
  }*/
}

void ButtonB_pressed() {
  emergencyStop = true;
  analogWrite(motorSpeed, 0);
  analogWrite(motorSpeed2, 0);
  /*if (digitalRead(PIN_BUTTON_B) == LOW) {                     // Buttom B is pressed
    //all motors stop ();
  }
  else {
    //press BUTTON_B to start the system ();
  }*/
}

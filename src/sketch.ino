//what's left: run back to read rfid tag
// reset the information about tag so it doesn't keep running after the tag is removed.
// reset possiton to null when it has read a tag.

// MultiStepper.pde
// -*- mode: C++ -*-
//
// Shows how to multiple simultaneous steppers
// Runs one stepper forwards and backwards, accelerating and decelerating
// at the limits. Runs other steppers at the same time
//
// Copyright (C) 2009 Mike McCauley
// $Id: MultiStepper.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define some steppers and the pins the will use
/* name one of them something with right and one left.
 * One should be inverse.
 */


/*
 * Test how HALF4WIRE works out instead of FULL4WIRE. Speed difference? Different 
 * step count to get one revolution - doubled?
 */

/*
 * Added 'false' as a last argument. will not enable the motors until it's
 * started with enableOutputs() later.
 */
AccelStepper stepper1(AccelStepper::HALF4WIRE, 3, 4, 5, 6);
AccelStepper stepper2(AccelStepper::HALF4WIRE, 8, 9, 10, 11);

int arm[] = {-1, -1};

const int STEPSPERREV = 400;

int x1, x2;

boolean firstRun = true;


// potentiometer variables.
int potLastRead[] = {0,0};
int potRead = 0;
int potPort[] = {2,3};
unsigned long lastPotRead = 0;


// rfid variables.
unsigned long lastRead;
int RFIDResetPin = 2;
int RFIDResetPin2 = 12;
int x1rfidtarget, x2rfidtarget;
// stump
char tag01[13] = "04161F27FDD7";
// stump
char tag02[13] = "04161F27F3D9";
// wheel
char tag03[13] = "04161F285471";
// wheel
char tag04[13] = "04161F271933";
int ledPin = 13;
char tagString[13];
int index = 0;
boolean reading = false;

void setup()
{  
  Serial.begin(9600);
  stepper1.setPinsInverted(true, true, true);
  stepper2.setPinsInverted(true, true, true);

	stepper1.setMaxSpeed(350.0);
	stepper1.setAcceleration(500.0);

	stepper2.setMaxSpeed(350.0);
	stepper2.setAcceleration(500.0);

  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, LOW);
  pinMode(RFIDResetPin2, OUTPUT);
  digitalWrite(RFIDResetPin2, LOW);
  pinMode(ledPin, OUTPUT);
}




void loop()
{
  if (millis() - lastRead > 10000 && millis() - lastPotRead > 2000) readCard();
  //readCard();
  

		x1 = constrain(map(analogRead(2), 520, 1010, 0, STEPSPERREV), 0, STEPSPERREV);
		x2 = constrain(map(analogRead(1), 530, 1010, 0, STEPSPERREV), 0, STEPSPERREV);
    /*
    x1 = analogRead(2);
		x2 = analogRead(1);
    */

    /*
    Serial.print(x1);
    Serial.print(" : ");
    Serial.print(x2);
    Serial.print(" : ");
    Serial.print(stepper1.targetPosition());
    Serial.print(" : ");
    Serial.print(stepper1.currentPosition());
    Serial.print(" :: ");
    Serial.print(stepper2.targetPosition());
    Serial.print(" : ");
    Serial.print(stepper2.currentPosition());

    Serial.println();
    */

    /*
    // Change direction at the limits
    if (stepper1.distanceToGo() == 0)
      stepper1.moveTo(-stepper1.currentPosition());
    if (stepper2.distanceToGo() == 0)
      stepper2.moveTo(-stepper2.currentPosition());
    */




    if (lastPotRead > 100 && firstRun == true) {
      firstRun = false;
      stepper1.setCurrentPosition(0);
      stepper2.setCurrentPosition(1);
    }
    if (millis() - lastPotRead > 1200 && !firstRun) {

      if (arm[1] == 2 || arm[1] == 3) {
        run(stepper1, true);
      } else if (arm[1] == 0 || arm[1] == 1) {
        run(stepper1, true);
      } else {
        if (x1 > 210 && x1 <= 380) {
          move(stepper1, potLastRead[0]);
        }
        else if (x1 > 20 && x1 <= 190) {
          move(stepper1, potLastRead[0]);
        }
        else if (x1 > 380){
          run(stepper1, true);
        } else if (x1 <= 20 ) {
          run(stepper1, false);
        } else if (x1 > 190 && x1 <= 210) {
          stop(stepper1);
        }
      }


      if (arm[0] == 2 || arm[0] == 3) {
        run(stepper2, true);
      } else if (arm[0] == 0 || arm[0] == 1) {
        run(stepper2, true);
      } else {
      
        if (x2 > 210 && x2 <= 380) {
          move(stepper2, potLastRead[1]);
        }
        else if (x2 > 20 && x2 <= 190) {
          move(stepper2, potLastRead[1]);
        }
        else if (x2 > 380) {
          run(stepper2, true);
        } else if (x2 <= 20 ) {
          run(stepper2, false);
        } else if (x2 > 190 && x2 <= 210) {
          stop(stepper2);
        }
      }
    }


  //Serial.println(abs(potLastRead[0] - x1) );
  if (abs(potLastRead[0] - x1) > 6) {
    if (stepper1.currentPosition() > 0) {
      stepper1.moveTo(x1);
    } else {
      stepper1.moveTo((x1) - STEPSPERREV);
    }
    potLastRead[0] = x1;
    lastPotRead = millis();
  }
  if (abs(potLastRead[1] - x2) > 6) {
    if (stepper2.currentPosition() > 0) {
      stepper2.moveTo(x2);
    } else {
      stepper2.moveTo((x2) - STEPSPERREV);
    }
    potLastRead[1] = x2;
    lastPotRead = millis();
  }
    if (arm[1] != -1) stepper1.run();
    if (arm[0] != -1) stepper2.run();
}




void readCard() {

  /*
   * This function need to connect different tags to different behavior. Store
   * tags and where to move to connected to those.
   */

  /*
   * Stops the motor if it does not find a rfid tag. (disableOutputs).
   * (or, don't start it again. start motors with enableOutputs when a rfid tag
   * is found.
   */

  /*
   * Need to use a led to confirm that a tag has been identified successfully when
   * running on battery and not reading the serial data.
   */


  /*
   * Reset position with setCurrentPosition(0) when a tag has been identified
   * successfully (0 is the position where it is possible to read a tag).
   */

  Serial.println("rfid_READ");
  index = 0;
  x1rfidtarget = stepper1.targetPosition();
  x2rfidtarget = stepper1.targetPosition();
  //stepper1.runToNewPosition(0);
  //stepper2.runToNewPosition(0);
  turnOffMotors(); // needs a turnonmotors as well when using disableOutputs.

  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin2, HIGH);
  delay(200);
  while(Serial.available()){

    int readByte = Serial.read(); //read next available byte

    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }
  // 0 == left
  checkTag(tagString, 0);
  clearTag(tagString);

  index = 0;
  digitalWrite(RFIDResetPin, HIGH);
  digitalWrite(RFIDResetPin2, LOW);
  delay(200);
  while(Serial.available()){

    int readByte = Serial.read(); //read next available byte

    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }

  // 1 == right
  checkTag(tagString, 1); //Check if it is a match
  clearTag(tagString); //Clear the char of all value
  digitalWrite(RFIDResetPin, LOW);

  //stepper1.runToNewPosition(x1rfidtarget);
  //stepper2.runToNewPosition(x2rfidtarget);


  turnOnMotors();


  lastRead = millis();

}

void move(AccelStepper &stepper, int pot) {
  pot = pot;
  if (stepper.targetPosition() < pot - 50 || stepper.targetPosition() > pot + 50 
    || stepper.currentPosition() == stepper.targetPosition()) {
    stepper.moveTo(pot + 50);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == pot + 50) {
    stepper.moveTo(pot - 50);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == pot - 50) {
    stepper.moveTo(pot + 50);
  }
}

void clearTag(char one[]){
///////////////////////////////////
//clear the char array by filling with null - ASCII 0
//Will think same tag has been read otherwise
///////////////////////////////////
  for(int i = 0; i < strlen(one); i++){
    one[i] = 0;
  }
}

void checkTag(char tag[], int armn){
///////////////////////////////////
//Check the read tag against known tags
///////////////////////////////////

  if(strlen(tag) == 0) return; //empty, no need to contunue

  if(compareTag(tag, tag01)){
    Serial.print("stump");
    arm[armn] = 0; // stump
  }
  if(compareTag(tag, tag02)){
    Serial.print("stump");
    arm[armn] = 1; // stump
  }
  if(compareTag(tag, tag03)){
    Serial.print("wheel");
    arm[armn] = 2; // wheel
  }
  if(compareTag(tag, tag04)){
    Serial.print("wheel");
    arm[armn] = 3; // wheel
  }
  else {
    arm[armn] = -1; // no tag
  }
  Serial.println(tag); //read out any unknown tag

}

boolean compareTag(char one[], char two[]){

  if(strlen(one) == 0) return false; //empty

  for(int i = 0; i < 12; i++){
    if(one[i] != two[i]) return false;
  }

  return true; //no mismatches
}



void lightLED(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////
  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}



void upperWave(AccelStepper &stepper) {
  /*
   * || stepper.currentPosition() == stepper.targetPosition()) {
   * This function needs this line when it's checking if it's moving in the
   * correct direction and in correct space.
   */

  if (stepper.targetPosition() < 25 || stepper.targetPosition() > 100) {
    stepper.moveTo(100);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == 100) {
    stepper.moveTo(25);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == 25)
    stepper.moveTo(100);
}

void downerWave(AccelStepper &stepper) {
  if (stepper.targetPosition() > 175 || stepper.targetPosition() < 100 
    || stepper.currentPosition() == stepper.targetPosition()) {
    stepper.moveTo(100);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == 100) {
    stepper.moveTo(175);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == 175)
    stepper.moveTo(100);
}

void stop(AccelStepper &stepper) {
  stepper.moveTo(stepper.currentPosition());
}

void run(AccelStepper &stepper, boolean forward) {
  if (forward) stepper.moveTo(STEPSPERREV);
  if (!forward) stepper.moveTo(-STEPSPERREV);
  if (stepper.distanceToGo() == 0) {
    stepper.setCurrentPosition(0);
  }
}

void turnOnMotors() {
  stepper1.enableOutputs();
  stepper2.enableOutputs();
}
void turnOffMotors() {
  ///*
  stepper1.disableOutputs();
  stepper2.disableOutputs();
  //*/
  /*
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  */
}

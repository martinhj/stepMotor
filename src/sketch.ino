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
AccelStepper stepper1(AccelStepper::FULL4WIRE, 3, 4, 5, 6);
AccelStepper stepper2(AccelStepper::FULL4WIRE, 8, 9, 10, 11);

int x1, x2;

unsigned long lastRead;
int RFIDResetPin = 2;
int RFIDResetPin2 = 12;
int x1rfidtarget, x2rfidtarget;
char tag10[13] = "04161F27FDD7";
int ledPin = 13;
char tagString[13];
int index = 0;
boolean reading = false;

void setup()
{  
  Serial.begin(9600);
  stepper1.setPinsInverted(true, true, true);
  stepper2.setPinsInverted(true, true, true);

	stepper1.setMaxSpeed(200.0);
	stepper1.setAcceleration(200.0);

	stepper2.setMaxSpeed(200.0);
	stepper2.setAcceleration(200.0);

  pinMode(RFIDResetPin, OUTPUT);
  pinMode(RFIDResetPin2, OUTPUT);
  pinMode(ledPin, OUTPUT);
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
   * successfully.
   */

  Serial.println("rfid_READ");
  index = 0;
  x1rfidtarget = stepper1.targetPosition();
  x2rfidtarget = stepper1.targetPosition();
  stepper1.runToNewPosition(0);
  stepper2.runToNewPosition(0);
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
  checkTag(tagString);
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

  checkTag(tagString); //Check if it is a match
  clearTag(tagString); //Clear the char of all value

  stepper1.runToNewPosition(x1rfidtarget);
  stepper2.runToNewPosition(x2rfidtarget);


  lastRead = millis();
}

void loop()
{
  if (millis() - lastRead > 10000) readCard();
  

		x1 = analogRead(2);
		x2 = analogRead(1);

    ///*
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
    //*/

    /*
    // Change direction at the limits
    if (stepper1.distanceToGo() == 0)
      stepper1.moveTo(-stepper1.currentPosition());
    if (stepper2.distanceToGo() == 0)
      stepper2.moveTo(-stepper2.currentPosition());
    */




    if (x1 > 800 && x1 <= 1000) {
      upperWave(stepper1);
    }
    else if (x1 > 550 && x1 <= 750) {
      downerWave(stepper1);
    }
    else if (x1 > 1000){
			run(stepper1, true);
		} else if (x1 < 550 ) {
      run(stepper1, false);
    } else if (x1 > 750 && x1 <= 800) {
      stop(stepper1);
    }
    if (x2 > 800 && x2 <= 1000) {
      upperWave(stepper2);
    }
    else if (x2 > 550 && x2 <= 750) {
      downerWave(stepper2);
    }
		else if (x2 > 1000) {
			run(stepper2, true);
		} else if (x2 < 550 ){
      run(stepper2, false);
    } else if (x2 > 750 && x2 <= 800) {
      stop(stepper2);
    }


    stepper1.run();
    stepper2.run();
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

void checkTag(char tag[]){
///////////////////////////////////
//Check the read tag against known tags
///////////////////////////////////

  if(strlen(tag) == 0) return; //empty, no need to contunue

  if(compareTag(tag, tag10)){
    lightLED(13);
  }
  Serial.println(tag); //read out any unknown tag

}

boolean compareTag(char one[], char two[]){
///////////////////////////////////
//compare two value to see if same,
//strcmp not working 100% so we do this
///////////////////////////////////

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
  Serial.println(pin);

  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}



void upperWave(AccelStepper &stepper) {
  /*
   * These functions needs to be generalized. Take in a start and a stop
   * position and state if it's going to keep rotating.
   */
  /*
   * || stepper.currentPosition() == stepper.targetPosition()) {
   * This function needs this line when it's checking if it's moving in the
   * correct direction and in correct space.
   */

  /* 
   * these functions needs to take potmeter vals in where it should move.
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
  if (forward) stepper.moveTo(200);
  if (!forward) stepper.moveTo(-200);
  if (stepper.distanceToGo() == 0) {
    stepper.setCurrentPosition(0);
  }
}

void turnOffMotors() {
  /*
   * void AccelStepper::disableOutputs  (   ) 
   * use this one instead (and enableOutputs).
   */
  /*
  stepper1.disableOutputs();
  stepper2.disableOutputs();
  */
  ///*
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  //*/
}


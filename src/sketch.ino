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


AccelStepper stepper1(AccelStepper::FULL4WIRE, 3, 4, 5, 6);
AccelStepper stepper2(AccelStepper::FULL4WIRE, 8, 9, 10, 11);

int x1, x2;

void setup()
{  
  Serial.begin(9600);
  stepper1.setPinsInverted(true, true, true);
  stepper2.setPinsInverted(true, true, true);

	stepper1.setMaxSpeed(200.0);
	stepper1.setAcceleration(200.0);

	stepper2.setMaxSpeed(200.0);
	stepper2.setAcceleration(200.0);
}



void loop()
{
  char tagString[13];
  int index = 0;
  boolean reading = false;

  delay(150);
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

		x1 = analogRead(2);
		x2 = analogRead(1);

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

  Serial.println(tag); //read out any unknown tag

}



void upperWave(AccelStepper &stepper) {
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
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}


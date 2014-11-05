//what's left: run back to read rfid tag
// reset the information about tag so it doesn't keep running after the tag is removed.
// reset possiton to null when it has read a tag.
// find a way to 'scan' the arm for tag's if it doesn't got any.

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
boolean firstRfidRead = false;


// potentiometer variables.
int potLastRead[] = {0,0};
int potRead = 0;
int potPort[] = {2,3};
unsigned long lastPotRead[] = {0,0};


// rfid variables.
unsigned long lastRead;
int RFIDResetPin = 2;
int RFIDResetPin2 = 12;
int x1rfidtarget, x2rfidtarget;
// stump
char tag01[13] = "04161F27FDD7";
// stump
char tag02[13] = "04161F27153F";
// wheel
char tag03[13] = "04161F27E4CE";
// wheel
char tag04[13] = "04161F271933";
// hand
char tag05[13] = "04161F268AA1";
// hand
char tag06[13] = "04161F27F3D9";
// glow
char tag07[13] = "04161F285471";
// glow
char tag08[13] = "04161F276E44";
int ledPin = 13;
char tagString[13];
int index = 0;
boolean reading = false;

void setup()
{  
  Serial.begin(9600);
  //stepper1.setPinsInverted(true, true, true);
  //stepper2.setPinsInverted(true, true, true);

	stepper1.setMaxSpeed(350.0);
	stepper1.setAcceleration(500.0);

	stepper2.setMaxSpeed(350.0);
	stepper2.setAcceleration(500.0);

  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, LOW);
  pinMode(RFIDResetPin2, OUTPUT);
  digitalWrite(RFIDResetPin2, LOW);
  pinMode(ledPin, OUTPUT);
  readCard();
}




void loop()
{
  if (millis() - lastRead > 10000 && millis() - (lastPotRead[0] > 2000 || lastPotRead[1] > 2000)) readCard();
  //readCard();
  

		x1 = constrain(map(analogRead(2), 520, 1010, 0, STEPSPERREV), STEPSPERREV, 0);
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




    if (((lastPotRead[0] > 100|| lastPotRead[1] > 100) || firstRfidRead == true) && firstRun == true) {
      firstRun = false;
      stepper1.setCurrentPosition(0);
      stepper2.setCurrentPosition(0);
    }
/*
    if (millis() - lastPotRead > 1200 && !firstRun) {

      if (arm[1] == 2 || arm[1] == 3) { // wheels
        run(stepper1, true);
      } else if (arm[1] == 0 || arm[1] == 1) { // stumps
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


      if (arm[0] == 2 || arm[0] == 3) { // wheels
        run(stepper2, true);
      } else if (arm[0] == 0 || arm[0] == 1) { // stumps
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
    */


  //Serial.println(abs(potLastRead[0] - x1) );
  potMove(stepper1, x1, 0);
  potMove(stepper2, x2, 1);

  
  if (millis() - lastPotRead[0] > 2200 && !firstRun) setPosition(stepper1, arm[0]);
  if (millis() - lastPotRead[1] > 2200 && !firstRun) setPosition(stepper2, arm[1]);



  stepper1.run();
  stepper2.run();
}



void potMove(AccelStepper &stepper, int potread, int pot) {
  if (abs(potLastRead[pot] - potread) > 6) {
    findPosition(stepper, potread);
    potLastRead[pot] = potread;
    lastPotRead[pot] = millis();
  }
}


void setPosition(AccelStepper &stepper, int state) {
  if (state == 0) stop(stepper);
  if (state == 1) run(stepper, true);
  if (state == 2) move(stepper, 90, 170);
  if (state == 3) run(stepper, false);
  if (state == 4) move(stepper, 0, 30);

}



void findPosition(AccelStepper &stepper, int potread) {
  int position = 0;
  if (potread >= 0 && potread <= 50) {
    position = 0;
    stateChangeMotor(stepper, 0);
  }
  if (potread > 50 && potread <= 150) { // straight forward
    position = 60;
    stateChangeMotor(stepper, 1);
  }
  if (potread > 150 && potread <= 250) { // wave
    position = 150;
    stateChangeMotor(stepper, 2);
  }
  if (potread > 250 && potread <= 350) { // straight backward
    position = 210;
    stateChangeMotor(stepper, 3);
  }
  if (potread > 350 && potread <= 400) {
    position = 250;
    stateChangeMotor(stepper, 4);
  }
  if (stepper.currentPosition() < 0) {
    stepper.moveTo(potread - STEPSPERREV);
  }
  stepper.moveTo(position);
}



void stateChangeMotor(AccelStepper &stepper, int state) {
  AccelStepper *steppera = &stepper;
  AccelStepper *stepperb = &stepper1; //right
  AccelStepper *stepperc = &stepper2; //left
  if (steppera == stepperb) {
    arm[0] = state;
    Serial.print("right arm ");
    Serial.print(state);
  }
  if (steppera == stepperc) {
    arm[1] = state;
    Serial.print("left arm ");
    Serial.print(state);
    Serial.println("      ");
  }
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
  checkTag(tagString, 1);
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
  checkTag(tagString, 0); //Check if it is a match
  clearTag(tagString); //Clear the char of all value
  digitalWrite(RFIDResetPin, LOW);

  //stepper1.runToNewPosition(x1rfidtarget);
  //stepper2.runToNewPosition(x2rfidtarget);


  turnOnMotors();


  lastRead = millis();

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

  if(strlen(tag) == 0) {
    Serial.println("no tag");
    return; //empty, no need to contunue
  }

  if(compareTag(tag, tag01)){
    Serial.print("stump");
    arm[armn] = 1; // stump
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);

  }
  if(compareTag(tag, tag02)){
    Serial.print("stump");
    arm[armn] = 1; // stump
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);
  }
  if(compareTag(tag, tag03)){
    Serial.print("wheel");
    arm[armn] = 1; // wheel
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);
  }
  if(compareTag(tag, tag04)){
    Serial.print("wheel");
    arm[armn] = 1; // wheel
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);
  }
  if(compareTag(tag, tag05)){ // hand
    Serial.print("hand");
    arm[armn] = 2; 
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);
  }
  if(compareTag(tag, tag06)){ // hand
    Serial.print("hand");
    arm[armn] = 2; 
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);
  }
  if(compareTag(tag, tag07)){ // glow
    Serial.print("glow");
    arm[armn] = 4; 
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);
  }
  if(compareTag(tag, tag08)){ // glow
    Serial.print("glow");
    arm[armn] = 4; 
    firstRfidRead = true;
    if (armn == 0) stepper1.setCurrentPosition(0);
    if (armn == 1) stepper2.setCurrentPosition(0);
  }
  /*else {*/
  /*  arm[armn] = 0; // no tag*/
  /*}*/
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



void move(AccelStepper &stepper, int start, int stop) {
  if (stepper.targetPosition() < start || stepper.targetPosition() > stop
    || stepper.currentPosition() == stepper.targetPosition()) {
    stepper.moveTo(start);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == start) {
    stepper.moveTo(stop);
  }
  if (stepper.distanceToGo() == 0 && stepper.targetPosition() == stop) {
    stepper.moveTo(start);
  }
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
  stepper1.disableOutputs();
  stepper2.disableOutputs();
}

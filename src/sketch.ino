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
boolean x1onedir, x2onedir;

void setup()
{  
	Serial.begin(9600);
	stepper1.setMaxSpeed(200.0);
	stepper1.setAcceleration(200.0);
	stepper1.moveTo(24);

	stepper2.setMaxSpeed(200.0);
	stepper2.setAcceleration(200.0);
	stepper2.moveTo(24);
}

void loop()
{
		x1 = analogRead(0);
		x2 = analogRead(1);

		Serial.print(x1);
		Serial.print(" : ");
		Serial.println(x2);
    // Change direction at the limits
    if (stepper1.distanceToGo() == 0)
      stepper1.moveTo(-stepper1.currentPosition());
    if (stepper2.distanceToGo() == 0)
      stepper2.moveTo(-stepper2.currentPosition());
    if (x1 < 1000){
      x1onedir = false;
			stepper1.run();
		} else {
      x1onedir = true;
			stepper1.runSpeed();
		}
		if (x2 < 1000) {
      x2onedir = false;
			stepper2.run();
		} else {
      x2onedir = true;
			stepper2.runSpeed();
		}
}


/*
void setSpeedAndDirection(int speed, int direction) {
}
*/

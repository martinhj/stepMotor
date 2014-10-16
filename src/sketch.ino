#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

int stepCount = 0;         // number of steps the motor has taken
boolean direction = false;

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one step:
  myStepper.setSpeed(100);
  if (direction = direction ? false : true) {
    myStepper.step(stepsPerRevolution * 2);
  } else {
    myStepper.step(-stepsPerRevolution * 2);
  }
  delay(500);
}

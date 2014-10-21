#include <Stepper.h>

const int stepsPerRevolution = 48;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

int stepCount = 0;         // number of steps the motor has taken
boolean direction = true;

void setup() {
  // initialize the serial port:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.println("starting");
  myStepper.setSpeed(20);
  myStepper.step(stepsPerRevolution);
  myStepper.setSpeed(40);
  myStepper.step(stepsPerRevolution);
  myStepper.setSpeed(200);
  myStepper.step(stepsPerRevolution);
}

void loop() {
  // step one step:
  if (direction = direction ? false : true) {
    myStepper.step(stepsPerRevolution);
    Serial.println("step+");
    digitalWrite(13, HIGH);
  } else {
    myStepper.step(-stepsPerRevolution);
    digitalWrite(13, LOW);
    Serial.println("step-");
  }
  delay(100);
}

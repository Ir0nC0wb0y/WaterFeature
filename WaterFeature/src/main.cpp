#include <Arduino.h>
#include <CheapStepper.h>


// Variables needed by CheapStepper
  CheapStepper pump_stepper(8,9,10,11);
  bool pump_stepper_dir = true;
  //unsigned long pump_stepper_moveStartTime = 0;



void setup() {
  pump_stepper.setRpm(12);

  Serial.begin(9600);

  Serial.print("stepper RPM: "); Serial.print(pump_stepper.getRpm());
  Serial.println();

  // and let's print the delay time (in microseconds) between each step
  // the delay is based on the RPM setting:
  // it's how long the stepper will wait before each step

  Serial.print("stepper delay (micros): "); Serial.print(pump_stepper.getDelay());
  Serial.println(); Serial.println();

  // now let's set up our first move...
  // let's move a half rotation from the start point

  pump_stepper.newMoveTo(pump_stepper_dir, 2048);
  /* this is the same as: 
   * stepper.newMoveToDegree(clockwise, 180);
   * because there are 4096 (default) steps in a full rotation
   */
  //pump_stepper_moveStartTime = millis(); // let's save the time at which we started this move
  
}

void loop() {
  // we need to call run() during loop() 
  // in order to keep the stepper moving
  // if we are using non-blocking moves
  pump_stepper.run();

  // let's check how many steps are left in the current move:
  
  int stepsLeft = pump_stepper.getStepsLeft();

  // if the current move is done...
  
  if (stepsLeft == 0){

    // let's print the position of the stepper to serial
    
    Serial.print("stepper position: "); Serial.print(pump_stepper.getStep());
    Serial.println();

    // and now let's print the time the move took

    //unsigned long timeTook = millis() - pump_stepper_moveStartTime; // calculate time elapsed since move start
    //Serial.print("move took (ms): "); Serial.print(timeTook);
    //Serial.println(); Serial.println();
    
    // let's start a new move in the reverse direction
    
    pump_stepper_dir = !pump_stepper_dir; // reverse direction
    pump_stepper.newMoveDegrees (pump_stepper_dir, 180); // move 180 degrees from current position
    //pump_stepper_moveStartTime = millis(); // reset move start time
  }

}
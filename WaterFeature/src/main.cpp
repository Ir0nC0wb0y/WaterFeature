#include <Arduino.h>
//#include <StepperDriver.h>


// Variables needed by StepperDriver
  // Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
  #define MOTOR_STEPS 200
  // Microstepping mode. If you hardwired it to save pins, set to the same value here.
  #define MICROSTEPS                          8
  #define DIR                                 8
  #define STEP                                9
  #define SLEEP                              13 // optional (just delete SLEEP from everywhere if not used)
  //#define MOTOR_ACCEL                      2000
  //#define MOTOR_DECEL                      1000
  #define STEPS_ROT    MICROSTEPS * MOTOR_STEPS
  #define RPM                                45
  
  #include "DRV8825.h"
  #define MODE0                              10
  #define MODE1                              11
  #define MODE2                              12
  DRV8825 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MODE0, MODE1, MODE2);
  

// Variables for timing
  int drip_qty                 =             5;              // [drips]
  unsigned long time_next_drip =             0;              // number of millis until next drip
  unsigned long time_last_drip;
  #define DRIP_QTY_MIN                       1               // [drips]
  #define DRIP_QTY_MAX                      20               // [drips]
  #define DRIP_PERIOD_MIN               300000               // [millis] or ~722ms per drop
  #define DRIP_PERIOD_MAX              1800000               // [millis]
  #define DRIP_DELAY_MAX                  2000               // [millis]
  #define STEPS_PER_DRIP                   232               // This is a guess for now
        // steps_turn = (4 * steps_rot*{microsteps} * Vd) / (pi^2 * ID_tube^2 * OD_pump)
        // Where:
        //  ID_tube   =  2 [mm]
        //  OD_pump   = 35 [mm]
        //  Vd (drop) = 50 [mm^3]
        //
        // Gives:
        //  steps_turn = 232 (rounded up)
        //  angle_turn ~  52 [degrees]



void setup() {
  stepper.begin(RPM,MICROSTEPS);
  stepper.enable();
  //stepper.setSpeedProfile(stepper.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);

  Serial.begin(9600);

  // Lets do a drip or 5 to start:
  Serial.print("Pushing "); Serial.print(drip_qty); Serial.println(" drops");
  // PUSH THE DROPS HERE
  time_last_drip = millis();
}

void loop() {
  // when there are no more steps, and the wait time has elapsed, do the drops
 bool check_steps = 0;
  if ( check_steps ) {
    bool check_time = 0;
    if ( check_time ) {
      time_next_drip = random(DRIP_PERIOD_MIN, DRIP_PERIOD_MAX);
      drip_qty       = random(   DRIP_QTY_MIN,    DRIP_QTY_MAX);
      Serial.print("Next drop sequence: (time_next_drip, drip_qty): "); Serial.print(time_next_drip); Serial.print(", "); Serial.println(drip_qty);
      while (drip_qty > 0) {
          Serial.print("drip ");
          stepper.move(STEPS_PER_DRIP);
          drip_qty = drip_qty - 1;
          delay(random(DRIP_DELAY_MAX));
      }
      time_last_drip = millis();
      stepper.disable();
      Serial.println();
    }
  }
}

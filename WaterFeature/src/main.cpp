#include <Arduino.h>


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
  #define RPM                                24
      // time for drip at this speed: .36 sec
  
  #include <DRV8825.h>
  #define MODE0                              10
  #define MODE1                              11
  #define MODE2                              12
  DRV8825 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MODE0, MODE1, MODE2);
  

// Variables for timing
  int drip_qty                 =             5;              // [drips]
  unsigned long time_next_drip =             0;              // number of millis until next drip
  unsigned long time_last_drip;
  #define DRIP_QTY_MIN                       1               // [drips]
  #define DRIP_QTY_MAX                       5               // [drips]
  #define DRIP_PERIOD_MIN               300000               // [millis] or  5 min
  #define DRIP_PERIOD_MAX              1800000               // [millis] or 30 min
  #define DRIP_DELAY_MIN                   250               // [millis]
  #define DRIP_DELAY_MAX                 10000               // [millis] or 10 sec
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
  stepper.move(STEPS_PER_DRIP * drip_qty);
  time_last_drip = millis();
}

void loop() {
  // when there are no more steps, and the wait time has elapsed, do the drops
  bool check_time = millis() - time_last_drip >= time_next_drip;
  if ( check_time ) {
    drip_qty = random(   DRIP_QTY_MIN,    DRIP_QTY_MAX);
    while (drip_qty > 0) {
        Serial.print("drip ");
        stepper.move(STEPS_PER_DRIP);
        drip_qty = drip_qty - 1;
        delay(random(DRIP_DELAY_MIN, DRIP_DELAY_MAX)); // randomize the drip rate/
    }
    time_last_drip = millis();
    stepper.disable();
    time_next_drip = random(DRIP_PERIOD_MIN, DRIP_PERIOD_MAX);
    Serial.print("Next drop in: "); Serial.print(time_next_drip); Serial.println(" ms");
    Serial.println();
  }
}

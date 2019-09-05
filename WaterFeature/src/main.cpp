#include <Arduino.h>
#include <CheapStepper.h>


// Variables needed by CheapStepper
  CheapStepper pump_stepper(8,9,10,11);
  bool pump_stepper_dir        =          true;
  #define STEPS_ROT                       4076

// Variables for timing
  int drip_qty                 =             2;              // [drips]
  unsigned long time_next_drip =             0;              // number of millis until next drip
  unsigned long time_last_drip;
  #define DRIP_QTY_MIN                       1               // [drips]
  #define DRIP_QTY_MAX                      20               // [drips]
  #define DRIP_PERIOD_MIN   DRIP_QTY_MAX * 722               // [millis] or ~722ms per drop
  #define DRIP_PERIOD_MAX                90000               // [millis]
  #define STEPS_PER_DRIP                   590               // This is a guess for now
        // steps_turn = (4 * steps_rot * Vd) / (pi^2 * ID_tube^2 * OD_pump)
        // Where:
        //  ID_tube   =  2 [mm]
        //  OD_pump   = 35 [mm]
        //  Vd (drop) = 50 [mm^3]
        //
        // Gives:
        //  steps_turn = 590 (rounded up)
        //  angle_turn ~ 52 [degrees]



void setup() {
  pump_stepper.setTotalSteps(STEPS_ROT);
  pump_stepper.setRpm(12);

  Serial.begin(9600);

  // Lets do a drip or 5 to start:
  Serial.println("Pushing 5 drops");
  pump_stepper.newMove(pump_stepper_dir, drip_qty * STEPS_PER_DRIP);
  time_last_drip = millis();
  
}

void loop() {
  // we need to call run() during loop() 
  // in order to keep the stepper moving
  // if we are using non-blocking moves
  pump_stepper.run();

  // when there are no more steps, and the wait time has elapsed, do the drops
  int stepsLeft = pump_stepper.getStepsLeft();
  bool check_steps = stepsLeft == 0;
  if ( check_steps ) {
    bool check_time = millis() - time_last_drip >= time_next_drip;
    if ( check_time ) {
      time_next_drip = random(DRIP_PERIOD_MIN, DRIP_PERIOD_MAX);
      drip_qty       = random(   DRIP_QTY_MIN,    DRIP_QTY_MAX);
      Serial.print("Next drop sequence: (time_next_drip, drip_qty): "); Serial.print(time_next_drip); Serial.print(", "); Serial.println(drip_qty);
      pump_stepper.newMove(pump_stepper_dir, drip_qty * STEPS_PER_DRIP);
      time_last_drip = millis();
    }
  }

}
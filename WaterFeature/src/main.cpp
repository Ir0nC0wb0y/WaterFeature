#include <Arduino.h>


// Variables needed by StepperDriver
  // Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
  #define MOTOR_STEPS                       200
  #define MICROSTEPS                          8  // DRV8825 will do up to 32 microsteps
  #define DIR                                 8
  #define STEP                                9
  #define SLEEP                               5 // optional (just delete SLEEP from everywhere if not used)
  #define STEPS_ROT    MICROSTEPS * MOTOR_STEPS
  #define RPM                                60 // pretty fast, but not motor's top speed

  #include <DRV8825.h>
  #define MODE0                              10
  #define MODE1                              11
  #define MODE2                              12
  DRV8825 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MODE0, MODE1, MODE2);
  

// Variables for timing
  int drip_qty                 =             5;              // [drips]
  unsigned long time_next_drip =             0;              // number of millis until next drip
  unsigned long time_last_drip;
  int drip_steps               =             0;
  #define DRIP_QTY_MIN                       1               // [drips]
  #define DRIP_QTY_MAX                       5               // [drips]
  #define DRIP_PERIOD_MIN                 1000
  #define DRIP_PERIOD_MAX                 5000
  //#define DRIP_PERIOD_MIN               300000               // [millis] or  5 min
  //#define DRIP_PERIOD_MAX              1800000               // [millis] or 30 min
  #define DRIP_DELAY_MIN                   250               // [millis]
  #define DRIP_DELAY_MAX                  3000               // [millis] or 10 sec

int steps_per_drip() {
  // This will be used to CALCULATE the number of steps per drip based on known constants

  double vd        =     50.0;
  double id_tube   =      2.0;
  double od_pump   =     35.0;
  
  //int turn_angle = (1440 * Vd)/(9.86960440109 * id_tube^2 * od_pump);

  int steps_per_drip = 0;
  steps_per_drip = (STEPS_ROT * 4 * vd) / ( 9.86960440109 * pow(id_tube,2) * od_pump);
  
  return steps_per_drip;
}

void do_drip() {
  digitalWrite(SLEEP,HIGH); // turn on motor
  stepper.move(drip_steps); // move motor by 1 drip
  delay(10);
  digitalWrite(SLEEP,LOW);  // turn off motor
}

void setup() {
  stepper.begin(RPM,MICROSTEPS);
  stepper.enable();
  
  pinMode(SLEEP,OUTPUT);
  digitalWrite(SLEEP,LOW);

  Serial.begin(9600);

  // calculate the number of steps per drip (used throughout)
  drip_steps = steps_per_drip();
  randomSeed(analogRead(0));

  // Lets do a drip or 5 to start:
  Serial.print("Pushing "); Serial.print(drip_qty); Serial.println(" drops");
  while ( drip_qty > 0) {
    do_drip();
    Serial.print("drip ");
    drip_qty--;
    if ( drip_qty > 0 ) {
      delay(250);
    }
  }
  Serial.println();
  time_last_drip = millis();
  time_next_drip = random(DRIP_PERIOD_MIN, DRIP_PERIOD_MAX);
  Serial.print("Next drop(s) in: "); Serial.print(time_next_drip); Serial.println(" ms");
  Serial.println();
}

void loop() {
  // when there are no more steps, and the wait time has elapsed, do the drops
  bool check_time = millis() - time_last_drip >= time_next_drip;
  if ( check_time ) {
    drip_qty = random(DRIP_QTY_MIN, DRIP_QTY_MAX);
    while (drip_qty > 0) {
        Serial.print("drip ");
        do_drip();
        drip_qty = drip_qty - 1;
        if (drip_qty > 0) {
          int drip_delay = random(DRIP_DELAY_MIN, DRIP_DELAY_MAX);
          Serial.print(drip_delay); Serial.print(" ");
          delay(drip_delay); // randomize the drip rate/
        }
    }
    Serial.println();
    time_last_drip = millis();
    stepper.disable();
    time_next_drip = random(DRIP_PERIOD_MIN, DRIP_PERIOD_MAX);
    Serial.print("Next drop(s) in: "); Serial.print(time_next_drip); Serial.println(" ms");
    Serial.println();
  }
}

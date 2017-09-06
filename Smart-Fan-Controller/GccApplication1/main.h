#ifndef MAIN_H_
#define MAIN_H_

//error states
emun Errors {
  NONE,
  BLOCKED,
  LOCKED
}

emun Errors ErrorStatus;

//Model of PWM Generator
struct PwmGenerator {
  uint8_t TOP;
  float dutyCycle;
}

struct SpeedController {

}

struct PowerMeasurement {

}

struct BlockedDuctDetection {

}

struct LockedRotorDetection {

}

struct CommunicationWithMaster {

}


#endif /* MAIN_H_ */

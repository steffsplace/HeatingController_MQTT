#ifndef VALVE_CONTROLLER_H
#define VALVE_CONTROLLER_H

#include <Arduino.h>

class ValveController {
public:
  ValveController(int openPin, int closePin);
  void begin();
  void update(float error);

private:
  enum State { Idle, MovingOpen, MovingClose };
  int _openPin, _closePin;
  State _state = Idle;
  unsigned long _untilMs = 0;
  unsigned long _lastEndMs = 0;
  unsigned long _cycleInterval = 60000; // ms between pulses
};

#endif

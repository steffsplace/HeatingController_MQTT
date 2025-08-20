#include "ValveController.h"
#include <math.h>

ValveController::ValveController(int openPin, int closePin)
: _openPin(openPin), _closePin(closePin) {}

void ValveController::begin() {
  pinMode(_openPin, OUTPUT);
  pinMode(_closePin, OUTPUT);
  digitalWrite(_openPin, LOW);
  digitalWrite(_closePin, LOW);
}

void ValveController::update(float error) {
  const unsigned long now = millis();

  // Finish movement when elapsed
  if (_state != Idle && (long)(now - _untilMs) >= 0) {
    digitalWrite(_openPin, LOW);
    digitalWrite(_closePin, LOW);
    _state = Idle;
    _lastEndMs = now;
  }

  // If moving or resting, do nothing
  if (_state != Idle) return;
  if ((now - _lastEndMs) < _cycleInterval) return;

  // Deadband ±0.5 °C, proportional pulse 2.0 s/°C clamped 3–20 s
  if (error > 0.5f) {
    int ms = (int)constrain((long)(fabsf(error) * 2000.0f), 3000L, 20000L);
    digitalWrite(_openPin, HIGH);
    digitalWrite(_closePin, LOW);
    _state = MovingOpen;
    _untilMs = now + (unsigned long)ms;
  } else if (error < -0.5f) {
    int ms = (int)constrain((long)(fabsf(error) * 2000.0f), 3000L, 20000L);
    digitalWrite(_openPin, LOW);
    digitalWrite(_closePin, HIGH);
    _state = MovingClose;
    _untilMs = now + (unsigned long)ms;
  }
}

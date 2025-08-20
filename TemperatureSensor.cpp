#include "TemperatureSensor.h"
#include <Adafruit_MAX31865.h>

Adafruit_MAX31865 rtd1 = Adafruit_MAX31865(10);
Adafruit_MAX31865 rtd2 = Adafruit_MAX31865(11);
Adafruit_MAX31865 rtd3 = Adafruit_MAX31865(12);

TemperatureSensor::TemperatureSensor(int csPin) : _csPin(csPin) {}

void TemperatureSensor::begin() {
  if (_csPin == 10) rtd1.begin(MAX31865_3WIRE);
  else if (_csPin == 11) rtd2.begin(MAX31865_3WIRE);
  else if (_csPin == 12) rtd3.begin(MAX31865_3WIRE);
}

float TemperatureSensor::readTemperature() {
  if (_csPin == 10) return rtd1.temperature(100.0, 430.0);
  if (_csPin == 11) return rtd2.temperature(100.0, 430.0);
  if (_csPin == 12) return rtd3.temperature(100.0, 430.0);
  return 0;
}

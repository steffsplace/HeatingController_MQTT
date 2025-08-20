#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

class TemperatureSensor {
public:
  TemperatureSensor(int csPin);
  void begin();
  float readTemperature();

private:
  int _csPin;
};

#endif

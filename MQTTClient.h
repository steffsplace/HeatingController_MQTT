#ifndef MQTT_CLIENT_WRAPPER_H
#define MQTT_CLIENT_WRAPPER_H

#include <Arduino.h>
#include <functional>
#include "Config.h"

class MQTTClientWrapper {
public:
  // Initialize Ethernet hardware and MQTT client (no broker connect yet)
  void begin();

  // Connect to broker using MQTT_BROKER_STR/MQTT_BROKER_PORT or provided args.
  bool connectBroker(const char* broker = MQTT_BROKER_STR, int port = MQTT_BROKER_PORT);

  // Call regularly from loop()
  void loop();

  // Publishing helpers
  void publishStatus(int index, float temp, float error);
  void publishTemperature(int index, float temp);

  // Incoming command handlers
  void onTargetTemperature(int index, std::function<void(float)> callback);
  void onPumpControl(int index, std::function<void(bool)> callback);

private:
  std::function<void(float)> _targetTempCallback[NUM_CIRCUITS] = { nullptr };
  std::function<void(bool)>  _pumpControlCallback[NUM_CIRCUITS] = { nullptr };

  // internal helpers
  void handleIncoming(const String& topic, const String& payload);
  void ensureConnected();

  unsigned long _lastConnectAttemptMs = 0;
};

extern MQTTClientWrapper mqtt;

#endif

/*
Version 3.2 Written by Steff Eberle GBS St. Gallen August 20ieth 2025

Implementation for the Arduino Machine Control Board on the Portenta H7 M7core

The current version of this Temperature control unit is capable of:
- controlling up to 3 Heating Cirquits
- communicating with a MQTT Broker via Ethernet/Internet
- communicating with a MQTT Broker to receive /subscribe the target temperature for each Heating Cirquit
- communicating with a MQTT Broker to receive /subscribe the HeatingPump activation for each Heating Cirquit
- doing Temperature Measurements /measures the actual Temperature of each Heating Cirquit with the HW on the Arduino Machine Control Board
- Controls the Valves of each Heating Cirquit...the open/close valves need to be operated in a duty cycle mode, as there is a time delay 
of valve adjustment and the effect of temperature adjustment
- communicating with a MQTT Broker to send/ publish the actual temperature of each Heating Cirquit
*/

#include "Config.h"
#include "TemperatureSensor.h"
#include "ValveController.h"
#include "MQTTClient.h"
#include <Arduino.h>

// Sensors and valves
TemperatureSensor sensors[NUM_CIRCUITS] = {
#if NUM_CIRCUITS >= 1
  TemperatureSensor(TEMP_SENSOR_1_CS),
#endif
#if NUM_CIRCUITS >= 2
  TemperatureSensor(TEMP_SENSOR_2_CS),
#endif
#if NUM_CIRCUITS >= 3
  TemperatureSensor(TEMP_SENSOR_3_CS),
#endif
};

ValveController valves[NUM_CIRCUITS] = {
#if NUM_CIRCUITS >= 1
  ValveController(VALVE1_OPEN_PIN, VALVE1_CLOSE_PIN),
#endif
#if NUM_CIRCUITS >= 2
  ValveController(VALVE2_OPEN_PIN, VALVE2_CLOSE_PIN),
#endif
#if NUM_CIRCUITS >= 3
  ValveController(VALVE3_OPEN_PIN, VALVE3_CLOSE_PIN),
#endif
};

// Default target temps
float targetTemps[NUM_CIRCUITS] = { 20.0, 20.0, 20.0 };

static void setTargetWrapper(int idx, float t) {
  if (idx >= 0 && idx < NUM_CIRCUITS) {
    targetTemps[idx] = t;
  }
}

static int pumpPins[NUM_CIRCUITS] = { PUMP1_DO_PIN, PUMP2_DO_PIN, PUMP3_DO_PIN };

static void pumpControlWrapper(int idx, bool on) {
  // idx is 0-based here
  if (idx >= 0 && idx < NUM_CIRCUITS) {
    digitalWrite(pumpPins[idx], on ? HIGH : LOW);
  }
  Serial.print("Pump "); Serial.print(idx+1); Serial.print(" -> "); Serial.println(on ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("HeatingController (Ethernet + ArduinoMqttClient) starting...");

  for (int i = 0; i < NUM_CIRCUITS; i++) {
    sensors[i].begin();
    valves[i].begin();
    pinMode(pumpPins[i], OUTPUT);
    digitalWrite(pumpPins[i], LOW);
    mqtt.onTargetTemperature(i, [i](float v){ setTargetWrapper(i, v); });
    mqtt.onPumpControl(i, [i](bool on){ pumpControlWrapper(i, on); });
  }

  mqtt.begin();

  // Initial broker connect
  if (!mqtt.connectBroker()) {
    Serial.println("Initial MQTT connect failed - will retry in loop");
  } else {
    Serial.println("MQTT connected");
  }
}

void loop() {
  mqtt.loop();

  static unsigned long lastSampleMs = 0;
  static unsigned long lastPublishMs = 0;
  unsigned long now = millis();

  if (now - lastSampleMs >= 500) {
    lastSampleMs = now;
    for (int i = 0; i < NUM_CIRCUITS; i++) {
      float current = sensors[i].readTemperature();
      float err = targetTemps[i] - current;
      valves[i].update(err);
    }
  }

  if (now - lastPublishMs >= 1000) {
    lastPublishMs = now;
    for (int i = 0; i < NUM_CIRCUITS; i++) {
      float current = sensors[i].readTemperature();
      float err = targetTemps[i] - current;
      mqtt.publishStatus(i + 1, current, err);
      mqtt.publishTemperature(i + 1, current);
    }
  }


  // Publish every 1000 ms
  if (now - lastPublishMs >= 1000) {
    lastPublishMs = now;
    for (int i = 0; i < NUM_CIRCUITS; i++) {
      float current = sensors[i].readTemperature();
      float err = targetTemps[i] - current;
      mqtt.publishStatus(i + 1, current, err);
      mqtt.publishTemperature(i + 1, current);
    }
  }


  delay(500);
}

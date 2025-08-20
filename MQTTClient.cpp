#include "MQTTClient.h"

#include <Arduino.h>
#include <Portenta_Ethernet.h>
#include <Ethernet.h>
#include <ArduinoMqttClient.h>
#include <IPAddress.h>

static EthernetClient ethClient;
static MqttClient mqttClient(ethClient);
static MQTTClientWrapper* instancePtr = nullptr;

// Global instance
MQTTClientWrapper mqtt;

void MQTTClientWrapper::begin() {
  instancePtr = this;

  IPAddress ip(STATIC_IP[0], STATIC_IP[1], STATIC_IP[2], STATIC_IP[3]);
  IPAddress dns(STATIC_DNS[0], STATIC_DNS[1], STATIC_DNS[2], STATIC_DNS[3]);
  IPAddress gw(STATIC_GATEWAY[0], STATIC_GATEWAY[1], STATIC_GATEWAY[2], STATIC_GATEWAY[3]);
  IPAddress sn(STATIC_SUBNET[0], STATIC_SUBNET[1], STATIC_SUBNET[2], STATIC_SUBNET[3]);

  // Initialize Ethernet with static addressing
  Ethernet.begin((uint8_t*)ETH_MAC, ip, dns, gw, sn);
  delay(1500); // allow link/PHY to settle

  // Reasonable keep-alive & buffer sizes can be tuned if needed
  mqttClient.setId("PortentaH7-heating");
  // If you need auth: mqttClient.setUsernamePassword("user", "pass");
}

bool MQTTClientWrapper::connectBroker(const char* broker, int port) {
  // Attempt connection (non-blocking style with timeout checks handled by caller loop)
  bool ok = mqttClient.connect(broker, port);
  if (!ok) {
    return false;
  }

  // Subscribe to setpoint and pump topics for each circuit
  for (int i = 0; i < NUM_CIRCUITS; i++) {
    char topic[96];
    snprintf(topic, sizeof(topic), MQTT_BASE_TOPIC "/target/%d", i + 1);
    mqttClient.subscribe(topic);
  }
  for (int i = 0; i < NUM_CIRCUITS; i++) {
    char topic[96];
    snprintf(topic, sizeof(topic), MQTT_BASE_TOPIC "/pump/%d", i + 1);
    mqttClient.subscribe(topic);
  }
  return true;
}

void MQTTClientWrapper::ensureConnected() {
  if (mqttClient.connected()) return;

  unsigned long now = millis();
  if (now - _lastConnectAttemptMs < 3000) return; // throttle attempts
  _lastConnectAttemptMs = now;

  connectBroker(MQTT_BROKER_STR, MQTT_BROKER_PORT);
}

void MQTTClientWrapper::loop() {
  ensureConnected();

  // Process incoming messages
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    String topic = mqttClient.messageTopic();
    String payload;
    while (mqttClient.available()) {
      payload += (char)mqttClient.read();
    }
    handleIncoming(topic, payload);
  }
}

void MQTTClientWrapper::publishStatus(int index, float temp, float error) {
  char topic[96];
  snprintf(topic, sizeof(topic), MQTT_BASE_TOPIC "/status/%d", index);

  char body[128];
  snprintf(body, sizeof(body), "{\"temp\":%.2f,\"error\":%.2f}", temp, error);

  mqttClient.beginMessage(topic);
  mqttClient.print(body);
  mqttClient.endMessage();
}

void MQTTClientWrapper::publishTemperature(int index, float temp) {
  char topic[96];
  snprintf(topic, sizeof(topic), MQTT_BASE_TOPIC "/temperature/%d", index);

  char body[32];
  snprintf(body, sizeof(body), "%.2f", temp);

  mqttClient.beginMessage(topic);
  mqttClient.print(body);
  mqttClient.endMessage();
}

void MQTTClientWrapper::onTargetTemperature(int index, std::function<void(float)> callback) {
  if (index >= 0 && index < NUM_CIRCUITS) {
    _targetTempCallback[index] = callback;
  }
}

void MQTTClientWrapper::onPumpControl(int index, std::function<void(bool)> callback) {
  if (index >= 0 && index < NUM_CIRCUITS) {
    _pumpControlCallback[index] = callback;
  }
}

void MQTTClientWrapper::handleIncoming(const String& topic, const String& payload) {
  const String base = String(MQTT_BASE_TOPIC) + "/";
  if (!topic.startsWith(base)) return;
  const String rest = topic.substring(base.length());
  int slash = rest.indexOf('/');
  if (slash < 0) return;
  const String head = rest.substring(0, slash);
  const String idxStr = rest.substring(slash + 1);
  int idx1 = idxStr.toInt();
  int idx = idx1 - 1;
  if (idx < 0 || idx >= NUM_CIRCUITS) return;
  if (head == "target") {
    float val = payload.toFloat();
    if (_targetTempCallback[idx]) _targetTempCallback[idx](val);
  } else if (head == "pump") {
    String p = payload; p.trim();
    bool on = (p == "1" || p.equalsIgnoreCase("on") || p == "true");
    if (_pumpControlCallback[idx]) _pumpControlCallback[idx](on);
  }
}

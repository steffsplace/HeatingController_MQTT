#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// ====== Project configuration ======
#define NUM_CIRCUITS 3

// Temperature sensor CS pins
#define TEMP_SENSOR_1_CS 10
#define TEMP_SENSOR_2_CS 11
#define TEMP_SENSOR_3_CS 12

// Valve control pins
#define VALVE1_OPEN_PIN 6
#define VALVE1_CLOSE_PIN 7
#define VALVE2_OPEN_PIN 5
#define VALVE2_CLOSE_PIN 4
#define VALVE3_OPEN_PIN 3
#define VALVE3_CLOSE_PIN 2

// Pump DO pins (set to your Machine Control digital outputs)
#define PUMP1_DO_PIN 22
#define PUMP2_DO_PIN 23
#define PUMP3_DO_PIN 24

// ====== Ethernet (STATIC) configuration ======
// Change these values to match your network
static const uint8_t ETH_MAC[6]        = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };
static const uint8_t STATIC_IP[4]      = { 192, 168, 1, 200 };
static const uint8_t STATIC_DNS[4]     = { 192, 168, 1, 1 };
static const uint8_t STATIC_GATEWAY[4] = { 192, 168, 1, 1 };
static const uint8_t STATIC_SUBNET[4]  = { 255, 255, 255, 0 };

// ====== MQTT settings ======
// Broker address as string (IP recommended for simplicity)
#define MQTT_BROKER_STR "192.168.1.50"
#define MQTT_BROKER_PORT 1883

// Base topic
#define MQTT_BASE_TOPIC "heating"

#endif

# Heating Controller (Portenta H7 + MQTT)
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
- Publishes status and temperatures for a Node‑RED dashboard.
*/


## Features
- Up to 3 circuits (`NUM_CIRCUITS` in code).
- MQTT (1‑based topics):
  - **Inputs (subscribe)**: `heating/target/1..3` (float °C), `heating/pump/1..3` (`0` or `1`)
  - **Outputs (publish)**: `heating/status/1..3` (JSON), `heating/temperature/1..3` (float)
- Non‑blocking valve pulses (no `delay()`), with deadband and rest-time between moves.
- Optional pump digital outputs (configure pins in `Config.h`).

## Hardware
- **Arduino Portenta H7** + **Machine Control** board
- 3x motorized valves (OPEN/CLOSE digital pins each)
- 0..3 pumps (digital outputs)
- RTD sensors via **Adafruit MAX31865** (PT100/PT1000). Adjust reference resistor and wiring mode in `TemperatureSensor` (e.g., `MAX31865_3WIRE`, `430.0` Ω).

## Project structure
- `*.ino` — main sketch (non‑blocking loop with `millis()` timers)
- `Config.h` — topics, pins, static IP/MAC (use `uint8_t` arrays), pump DO pins
- `MQTTClient.h/.cpp` — Ethernet + ArduinoMqttClient wrapper, topic subscriptions, message routing
- `ValveController.h/.cpp` — valve state machine (Idle/Open/Close), proportional pulse timing
- `TemperatureSensor.*` — MAX31865 integration (adjust PT type and Rref)
- `node_red_heating_flow.json` — Node‑RED dashboard (sliders, switches, status, charts)

## MQTT topics
- **Set targets**: `heating/target/1`, `heating/target/2`, `heating/target/3`  (payload e.g. `21.5`)
- **Set pump ON/OFF**: `heating/pump/1`, `heating/pump/2`, `heating/pump/3`  (payload `"1"` / `"0"`)
- **Status JSON** (controller → broker): `heating/status/1..3`
  ```json
  {"actual": 20.8, "target": 21.5, "pump": 1, "state": "open"}
  ```
- **Temperature** (controller → broker): `heating/temperature/1..3` (payload float like `20.80`)

## Build (Arduino IDE 2.x)
**Libraries** (Library Manager):
- **ArduinoMqttClient**
- **Ethernet** (or **Portenta_Ethernet**, depending on your setup)
- **Adafruit MAX31865**

Board: Portenta H7 (M7 core).
Adjust `Config.h` for network (MAC/IP) and pins.

## Node‑RED
Import `node_red_heating_flow.json` and point the broker to your MQTT host (default `192.168.1.50`).
Dashboard provides target sliders, pump switches, status text, and temperature charts.

## Quick start
1. Edit `Config.h`:
   - `MQTT_BASE_TOPIC` (default `"heating"`)
   - Network: MAC/IP/DNS/GW/Subnet (as `uint8_t` arrays)
   - Valve pins and `PUMPx_DO_PIN` pins
2. Flash the sketch.
3. Start MQTT broker (e.g., Mosquitto).
4. Import Node‑RED flow and deploy.
5. Adjust targets and pumps from the dashboard.

## License
MIT (or choose your preferred license).

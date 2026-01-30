#pragma once

#include <stdint.h>

// --- SPI Pins (ESP32 -> MCP2515) ---
// Same pin config as github.com/ianc99/BMW-GS-Wonder-Wheel-map-zoomer
#define MCP2515_SCK_PIN  26
#define MCP2515_MISO_PIN 14
#define MCP2515_MOSI_PIN 27
#define MCP2515_CS_PIN   12
#define MCP2515_INT_PIN  25

// --- CAN Bus Speed ---
// BMW F800GT uses 500 kbps
// MCP2515 crystal: 8 MHz (common on most modules)
#define MCP2515_CRYSTAL MCP_8MHZ

// --- OBD-II CAN IDs ---
#define OBD2_REQUEST_ID  0x7DF  // Broadcast request
#define OBD2_RESPONSE_ID 0x7E8  // ECU response

// --- OBD-II Service ---
#define OBD2_SERVICE_CURRENT_DATA 0x01

// --- OBD-II PIDs ---
#define PID_COOLANT_TEMP     0x05  // Temperatura refrigerante (°C)
#define PID_ENGINE_RPM       0x0C  // RPM motor
#define PID_VEHICLE_SPEED    0x0D  // Velocidad (km/h)
#define PID_INTAKE_AIR_TEMP  0x0F  // Temperatura aire admision (°C)
#define PID_THROTTLE_POS     0x11  // Posicion acelerador (%)
#define PID_FUEL_LEVEL       0x2F  // Nivel combustible (%)

// List of PIDs to poll
const uint8_t PIDS_TO_POLL[] = {
    PID_COOLANT_TEMP,
    PID_ENGINE_RPM,
    PID_VEHICLE_SPEED,
    PID_INTAKE_AIR_TEMP,
    PID_THROTTLE_POS,
    PID_FUEL_LEVEL,
};
const uint8_t NUM_PIDS = sizeof(PIDS_TO_POLL) / sizeof(PIDS_TO_POLL[0]);

// --- Timing ---
#define POLL_INTERVAL_MS       1000  // Intervalo entre ciclos de lectura
#define CAN_RECEIVE_TIMEOUT_MS 100   // Timeout para recibir respuesta CAN

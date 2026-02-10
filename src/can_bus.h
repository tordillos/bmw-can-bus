#pragma once

#include <stdint.h>

// Initialize the ESP32 TWAI CAN controller. Returns true on success.
bool can_init();

// Send a CAN frame. Returns true on success.
bool can_send(uint32_t id, const uint8_t *data, uint8_t len);

// Receive a CAN frame. Returns true if a message was received within timeout_ms.
// Populates id, data buffer, and len.
bool can_receive(uint32_t &id, uint8_t *data, uint8_t &len, uint32_t timeout_ms);

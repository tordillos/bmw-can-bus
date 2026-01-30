#pragma once

#include <stdint.h>

// Send an OBD-II mode 0x01 request for the given PID.
// Returns true if the CAN frame was sent successfully.
bool obd2_request(uint8_t pid);

// Read an OBD-II response. Populates pid, data buffer, and data_len.
// Returns true if a valid response was received within timeout.
bool obd2_read_response(uint8_t &pid, uint8_t *data, uint8_t &data_len);

// Parse and print a human-readable value for the given PID and raw data.
void obd2_print_value(uint8_t pid, const uint8_t *data, uint8_t data_len);

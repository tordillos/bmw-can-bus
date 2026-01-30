#include "obd2.h"
#include "can_bus.h"
#include "config.h"
#include <Arduino.h>

bool obd2_request(uint8_t pid) {
    uint8_t data[8] = {
        0x02,                        // Number of additional bytes: 2
        OBD2_SERVICE_CURRENT_DATA,   // Service 0x01
        pid,                         // Requested PID
        0x00, 0x00, 0x00, 0x00, 0x00 // Padding
    };
    return can_send(OBD2_REQUEST_ID, data, 8);
}

bool obd2_read_response(uint8_t &pid, uint8_t *data, uint8_t &data_len) {
    uint32_t rx_id;
    uint8_t rx_data[8];
    uint8_t rx_len;

    if (!can_receive(rx_id, rx_data, rx_len, CAN_RECEIVE_TIMEOUT_MS)) {
        return false;
    }

    // Check for standard OBD-II response ID
    if (rx_id != OBD2_RESPONSE_ID) {
        return false;
    }

    // rx_data[0] = number of additional bytes
    // rx_data[1] = service response (0x41 for service 0x01)
    // rx_data[2] = PID
    // rx_data[3..] = PID data
    if (rx_len < 3 || rx_data[1] != 0x41) {
        return false;
    }

    uint8_t extra_bytes = rx_data[0];
    pid = rx_data[2];
    data_len = extra_bytes - 2; // subtract service + PID bytes
    if (data_len > 5) data_len = 5; // safety clamp

    memcpy(data, &rx_data[3], data_len);
    return true;
}

void obd2_print_value(uint8_t pid, const uint8_t *data, uint8_t data_len) {
    switch (pid) {
        case PID_COOLANT_TEMP:
            if (data_len >= 1) {
                int temp = (int)data[0] - 40;
                Serial.printf("Coolant Temp:   %d °C\n", temp);
            }
            break;

        case PID_ENGINE_RPM:
            if (data_len >= 2) {
                float rpm = ((uint16_t)data[0] * 256 + data[1]) / 4.0f;
                Serial.printf("Engine RPM:     %.0f\n", rpm);
            }
            break;

        case PID_VEHICLE_SPEED:
            if (data_len >= 1) {
                Serial.printf("Speed:          %d km/h\n", data[0]);
            }
            break;

        case PID_INTAKE_AIR_TEMP:
            if (data_len >= 1) {
                int temp = (int)data[0] - 40;
                Serial.printf("Intake Air:     %d °C\n", temp);
            }
            break;

        case PID_THROTTLE_POS:
            if (data_len >= 1) {
                float pct = data[0] * 100.0f / 255.0f;
                Serial.printf("Throttle:       %.1f%%\n", pct);
            }
            break;

        case PID_FUEL_LEVEL:
            if (data_len >= 1) {
                float pct = data[0] * 100.0f / 255.0f;
                Serial.printf("Fuel Level:     %.1f%%\n", pct);
            }
            break;

        default:
            Serial.printf("PID 0x%02X:       ", pid);
            for (uint8_t i = 0; i < data_len; i++) {
                Serial.printf("%02X ", data[i]);
            }
            Serial.println();
            break;
    }
}

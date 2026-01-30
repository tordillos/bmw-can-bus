#include <Arduino.h>
#include "config.h"
#include "can_bus.h"
#include "obd2.h"

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== BMW F800GT CAN Bus Reader ===");

    if (!can_init()) {
        Serial.println("ERROR: CAN bus init failed. Check wiring.");
        while (true) { delay(1000); }
    }
}

void loop() {
    Serial.println("--- Reading OBD-II PIDs ---");

    for (uint8_t i = 0; i < NUM_PIDS; i++) {
        uint8_t pid = PIDS_TO_POLL[i];

        if (obd2_request(pid)) {
            uint8_t resp_pid;
            uint8_t data[5];
            uint8_t data_len;

            if (obd2_read_response(resp_pid, data, data_len)) {
                obd2_print_value(resp_pid, data, data_len);
            } else {
                Serial.printf("PID 0x%02X: no response\n", pid);
            }
        }

        delay(50);
    }

    Serial.println();
    delay(POLL_INTERVAL_MS);
}

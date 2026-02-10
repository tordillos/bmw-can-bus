#include "can_bus.h"
#include "config.h"
#include <Arduino.h>
#include <driver/twai.h>

bool can_init() {
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        Serial.println("TWAI driver install failed");
        return false;
    }

    if (twai_start() != ESP_OK) {
        Serial.println("TWAI start failed");
        twai_driver_uninstall();
        return false;
    }

    Serial.printf("CAN bus initialized (TWAI, 500 kbps) TX=%d RX=%d\n", CAN_TX_PIN, CAN_RX_PIN);
    return true;
}

bool can_send(uint32_t id, const uint8_t *data, uint8_t len) {
    twai_message_t msg = {};
    msg.identifier = id;
    msg.data_length_code = len;
    memcpy(msg.data, data, len);

    if (twai_transmit(&msg, pdMS_TO_TICKS(100)) != ESP_OK) {
        Serial.printf("CAN TX failed (id=0x%03lX)\n", id);
        return false;
    }
    return true;
}

bool can_receive(uint32_t &id, uint8_t *data, uint8_t &len, uint32_t timeout_ms) {
    twai_message_t msg;

    if (twai_receive(&msg, pdMS_TO_TICKS(timeout_ms)) == ESP_OK) {
        id = msg.identifier;
        len = msg.data_length_code;
        memcpy(data, msg.data, len);
        return true;
    }
    return false;
}

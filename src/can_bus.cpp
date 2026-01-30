#include "can_bus.h"
#include "config.h"
#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>

static MCP_CAN CAN0(MCP2515_CS_PIN);

bool can_init() {
    Serial.printf("SPI pins: SCK=%d, MISO=%d, MOSI=%d, CS=%d, INT=%d\n",
                  MCP2515_SCK_PIN, MCP2515_MISO_PIN, MCP2515_MOSI_PIN,
                  MCP2515_CS_PIN, MCP2515_INT_PIN);

    SPI.begin(MCP2515_SCK_PIN, MCP2515_MISO_PIN, MCP2515_MOSI_PIN, MCP2515_CS_PIN);

    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
        Serial.println("MCP2515: init failed");
        return false;
    }

    CAN0.setMode(MCP_NORMAL);
    pinMode(MCP2515_INT_PIN, INPUT);

    Serial.println("CAN bus initialized (MCP2515, 500 kbps)");
    return true;
}

bool can_send(uint32_t id, const uint8_t *data, uint8_t len) {
    byte ret = CAN0.sendMsgBuf(id, 0, len, (byte *)data);
    if (ret != CAN_OK) {
        Serial.printf("CAN TX failed (id=0x%03lX): error %d\n", id, ret);
        return false;
    }
    return true;
}

bool can_receive(uint32_t &id, uint8_t *data, uint8_t &len, uint32_t timeout_ms) {
    unsigned long start = millis();

    while (millis() - start < timeout_ms) {
        if (!digitalRead(MCP2515_INT_PIN)) {
            long unsigned int rxId;
            byte rxLen;
            if (CAN0.readMsgBuf(&rxId, &rxLen, data) == CAN_OK) {
                id = rxId;
                len = rxLen;
                return true;
            }
        }
        delay(1);
    }
    return false;
}

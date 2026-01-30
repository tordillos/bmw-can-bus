#include "bt_serial.h"
#include "config.h"
#include <Arduino.h>

BluetoothSerial SerialBT;

bool bt_init() {
    if (!SerialBT.begin(BT_DEVICE_NAME)) {
        Serial.println("Bluetooth: init failed");
        return false;
    }

    Serial.printf("Bluetooth: ready as \"%s\"\n", BT_DEVICE_NAME);
    return true;
}

bool bt_connected() {
    return SerialBT.connected();
}

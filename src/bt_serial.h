#pragma once

#include <BluetoothSerial.h>

// Initialize Bluetooth Serial SPP with the configured device name.
// Returns true on success.
bool bt_init();

// Returns true if a Bluetooth client is currently connected.
bool bt_connected();

// Global Bluetooth serial instance for output.
extern BluetoothSerial SerialBT;

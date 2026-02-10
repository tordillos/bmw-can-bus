# CLAUDE.md

This file provides guidance for AI assistants working with this codebase.

## Project Overview

ESP32-based OBD-II diagnostic reader for BMW F800GT motorcycles. Reads engine data via the CAN bus protocol using the ESP32's native TWAI controller with an SN65HVD230 CAN transceiver, and outputs human-readable values over serial and Bluetooth SPP.

**Target hardware:** ESP32 HW-395 (CP2102) + SN65HVD230 CAN transceiver module
**Vehicle:** BMW F800GT (CAN bus at 500 kbps)
**Framework:** Arduino on PlatformIO

## Repository Structure

```
├── include/
│   └── config.h            # All hardware pins, CAN/OBD-II constants, timing
├── src/
│   ├── main.cpp            # Entry point: setup/loop, PID polling cycle
│   ├── can_bus.h            # CAN abstraction interface (init, send, receive)
│   ├── can_bus.cpp          # ESP32 TWAI driver for CAN communication
│   ├── obd2.h              # OBD-II protocol interface (request, read, print)
│   ├── obd2.cpp            # OBD-II frame construction, response parsing, value formatting
│   ├── bt_serial.h         # Bluetooth SPP interface (init, connection check)
│   └── bt_serial.cpp       # Bluetooth Serial setup using ESP32 BluetoothSerial
├── platformio.ini           # PlatformIO build configuration
├── .gitignore
└── README.md               # Hardware setup and usage guide (Spanish)
```

## Architecture

The code is layered into four levels:

1. **CAN bus layer** (`can_bus.*`) -- ESP32 native TWAI controller communication via SN65HVD230 transceiver. Handles init, send, and receive with timeout.
2. **OBD-II protocol layer** (`obd2.*`) -- Constructs OBD-II mode 0x01 request frames, validates responses (CAN ID 0x7E8, service 0x41), and parses PID-specific data. Output goes to both Serial and Bluetooth via `dual_printf()`.
3. **Bluetooth layer** (`bt_serial.*`) -- ESP32 Bluetooth Classic SPP. Exposes the device as a serial port that mobile apps can connect to. No additional hardware required.
4. **Application layer** (`main.cpp`) -- Iterates through configured PIDs, sends requests, reads responses, and prints formatted values on a 1-second polling cycle.

All configuration constants live in `include/config.h` -- pin definitions, CAN speed, OBD-II IDs, PID list, and timing values.

## Build Commands

This project uses PlatformIO. The `pio` CLI must be installed.

```bash
pio run                  # Compile
pio run -t upload        # Compile and flash to ESP32
pio device monitor       # Open serial monitor (115200 baud)
pio run -t clean         # Clean build artifacts
pio run -t erase         # Erase ESP32 flash
```

Build artifacts are stored in `.pio/` (gitignored).

## Dependencies

No external libraries required. The TWAI driver (`driver/twai.h`) and Bluetooth (`BluetoothSerial.h`) are provided by the `espressif32` platform (ESP-IDF).

## Hardware Pin Configuration

Defined in `include/config.h`:

| ESP32 Pin | SN65HVD230 Pin | Function |
|-----------|----------------|----------|
| GPIO 4    | CTX            | CAN TX   |
| GPIO 5    | CRX            | CAN RX   |
| 3V3       | 3V3            | Power    |
| GND       | GND            | Ground   |

## OBD-II PIDs Supported

Configured in `config.h` via the `PIDS_TO_POLL` array:

| PID | Parameter | Formula |
|-----|-----------|---------|
| 0x05 | Coolant Temperature | `value - 40` (degrees C) |
| 0x0C | Engine RPM | `(A*256 + B) / 4` |
| 0x0D | Vehicle Speed | Direct value (km/h) |
| 0x0F | Intake Air Temperature | `value - 40` (degrees C) |
| 0x11 | Throttle Position | `value * 100 / 255` (%) |
| 0x2F | Fuel Level | `value * 100 / 255` (%) |

## Coding Conventions

- **Language:** C++ with Arduino framework idioms
- **Header guards:** `#pragma once`
- **Naming:**
  - `CAN_*` prefix for CAN bus pin and timing constants
  - `PID_*` prefix for OBD-II parameter IDs
  - `OBD2_*` prefix for OBD-II protocol constants
  - `BT_*` prefix for Bluetooth constants
  - `bt_*` prefix for Bluetooth functions
  - Functions use `snake_case` (e.g., `can_init()`, `obd2_request()`, `bt_init()`)
  - Constants and macros use `UPPER_SNAKE_CASE`
- **Error handling:** Functions return `bool` for success/failure
- **Parameter passing:** Output parameters use references (`uint8_t &len`); input buffers use `const` pointers
- **Comments:** Code comments in English; README documentation in Spanish
- **File organization:** Each module has a `.h` (interface) and `.cpp` (implementation) pair; all config in `config.h`

## Testing

There are no automated tests. Verification is done manually via serial monitor output:

- **Without motorcycle connected:** All PID requests show "no response"
- **With motorcycle connected:** Values print in human-readable format (e.g., `Coolant Temp: 85 degrees C`)
- **Bluetooth:** Pair a mobile device with "BMW-F800GT-OBD" and open a Bluetooth serial terminal app to see the same output

## Key Technical Details

- CAN bus speed is 500 kbps (BMW standard)
- Uses ESP32 native TWAI (Two-Wire Automotive Interface) controller — no external CAN controller needed
- SN65HVD230 is a 3.3V CAN transceiver; no level shifting or series resistors required
- OBD-II requests use broadcast ID `0x7DF`; responses come from ECU at `0x7E8`
- Response parsing validates service byte (`0x41`) and clamps data length to 5 bytes max
- Polling interval is 1000 ms between cycles, with 50 ms between individual PID requests and 100 ms receive timeout
- TWAI receive uses FreeRTOS tick-based blocking (no pin polling)
- Bluetooth uses ESP32 Classic SPP (Serial Port Profile); device name is configurable via `BT_DEVICE_NAME` in `config.h`
- Bluetooth output is only sent when a client is connected (`bt_connected()` check); no overhead when disconnected

## Adding a New OBD-II PID

1. Add the PID constant in `include/config.h` (e.g., `#define PID_NEW_PARAM 0xXX`)
2. Add it to the `PIDS_TO_POLL` array in `config.h`
3. Add a `case` in `obd2_print_value()` in `src/obd2.cpp` with the appropriate formula
4. Rebuild with `pio run`

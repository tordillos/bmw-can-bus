# BMW F800GT CAN Bus Reader

Lector OBD-II para BMW F800GT usando ESP32 HW-395 CP2102 (38 pines) y modulo MCP2515 + TJA1050.

Configuracion de pines basada en [BMW-GS-Wonder-Wheel-map-zoomer](https://github.com/ianc99/BMW-GS-Wonder-Wheel-map-zoomer).

## Hardware necesario

- ESP32 HW-395 CP2102 38 pines
- Modulo MCP2515 + TJA1050 (controlador CAN + transceiver)
- 7 cables dupont hembra-hembra (usar cables de buena calidad o soldar)
- 1 resistencia 4.7kΩ (recomendada para el cable SO)
- Cable USB con datos (no solo de carga)

## Conexiones

### Tabla de cableado

Todos los cables van al **lado derecho** de la placa (con el USB arriba).

| Pin ESP32 (etiqueta en placa) | Pin MCP2515 | Funcion      | Notas                          |
|-------------------------------|-------------|--------------|--------------------------------|
| P27                           | SI          | SPI MOSI     | Directo                        |
| P14                           | SO          | SPI MISO     | Con resistencia 4.7kΩ en serie |
| P26                           | SCK         | SPI Clock    | Directo                        |
| P12                           | CS          | Chip Select  | Directo                        |
| P25                           | INT         | Interrupcion | Directo                        |
| 5V                            | VCC         | Alimentacion | Directo                        |
| GND                           | GND         | Tierra       | Directo                        |

Del MCP2515 a la moto:

| Pin MCP2515 | BMW F800GT (conector diag.) |
|-------------|-----------------------------|
| H           | CAN-H                       |
| L           | CAN-L                       |

### Resistencia en SO (MISO)

El MCP2515 trabaja a 5V y su pin SO emite señales de 5V. Se recomienda una resistencia de 4.7kΩ en serie para proteger el ESP32:

```
MCP2515 SO ──── [4.7kΩ] ──── ESP32 P14
```

Vale cualquier resistencia entre 3.3kΩ y 10kΩ. Puede funcionar sin ella, pero no es recomendable a largo plazo.

### Pinout ESP32 HW-395 (38 pines)

Los pines marcados con `◄` son los que hay que conectar al MCP2515.
**Todos van al lado derecho** de la placa:

```
                         USB
                    ┌────┤├────┐
              CLK   │          │  5V     ◄── a VCC
              SD0   │          │  CMD
              SD1   │          │  SD3
              P15   │          │  SD2
               P2   │          │  P13
               P0   │          │  GND    ◄── a GND
               P4   │  HW-395 │  P12    ◄── a CS
              P16   │          │  P14    ◄── a SO (con 4.7kΩ)
              P17   │          │  P27    ◄── a SI
               P5   │          │  P26    ◄── a SCK
              P18   │          │  P25    ◄── a INT
              P19   │          │  P33
              GND   │          │  P32
              P21   │          │  P35
               RX   │          │  P34
               TX   │          │  SVN
              P22   │          │  SVP
              P23   │          │  EN
              GND   │          │  3V3
                    └───────────┘
```

### Calidad de los cables

Los cables dupont baratos dan mal contacto y causan fallos intermitentes. Para uso en la moto se recomienda:

- Soldar las conexiones directamente
- O usar cables dupont de buena calidad y fijarlos con termorretractil

## Instalacion

### 1. Instalar PlatformIO

```bash
pip3 install platformio
```

O con Homebrew (macOS):

```bash
brew install platformio
```

### 2. Driver USB

La placa usa el chip **CP2102**. Si macOS no la detecta automaticamente, instala el driver:

```bash
brew install silicon-labs-vcp-driver
```

Despues de instalar, desconecta y reconecta el USB.

Verifica que el ESP32 aparece:

```bash
ls /dev/cu.usb*
```

Deberia aparecer algo como `/dev/cu.usbserial-0001`.

## Compilar y subir

### 1. Compilar

```bash
pio run
```

La primera vez descargara el toolchain de ESP32 y la libreria MCP_CAN automaticamente.

### 2. Flashear al ESP32

```bash
pio run -t upload
```

Si no detecta el puerto automaticamente:

```bash
pio device list
pio run -t upload --upload-port /dev/cu.usbserial-XXXX
```

### 3. Si el flash esta corrupto

Si ves `csum err` o `flash read err` en el monitor serial, borra el flash y vuelve a subir:

```bash
pio run -t erase && pio run -t upload
```

### 4. Si no conecta con el ESP32

Si aparece `Failed to connect to ESP32: No serial data received`:

1. Ejecuta `pio run -t upload`
2. Cuando veas `Connecting........`, **manten pulsado el boton BOOT** de la placa
3. Sueltalo cuando empiece a subir el firmware

Si no funciona, prueba la secuencia completa:

1. Manten pulsado **BOOT**
2. Pulsa y suelta **EN** (reset) sin soltar BOOT
3. Suelta **BOOT**
4. Ejecuta `pio run -t upload`

El boton **BOOT** es el que esta mas cerca del puerto USB (puede decir `BOOT`, `IO0` o `0`).

## Monitor serial

Para ver la salida del ESP32 en tiempo real:

```bash
pio device monitor
```

Salida esperada con el MCP2515 conectado (sin la moto):

```
=== BMW F800GT CAN Bus Reader ===
SPI pins: SCK=26, MISO=14, MOSI=27, CS=12, INT=25
Entering Configuration Mode Successful!
Setting Baudrate Successful!
CAN bus initialized (MCP2515, 500 kbps)
--- Reading OBD-II PIDs ---
PID 0x05: no response
PID 0x0C: no response
...
```

Los "no response" son normales sin la moto conectada. Al conectar H y L al conector de diagnostico de la BMW F800GT con el motor encendido, deberian aparecer los valores.

Salida esperada con la moto conectada:

```
Coolant Temp:   85 °C
Engine RPM:     3200
Speed:          60 km/h
Intake Air:     35 °C
Throttle:       15.3%
Fuel Level:     72.5%
```

## PIDs OBD-II soportados

| PID  | Descripcion              |
|------|--------------------------|
| 0x05 | Temperatura refrigerante |
| 0x0C | RPM motor                |
| 0x0D | Velocidad                |
| 0x0F | Temperatura aire admision|
| 0x11 | Posicion acelerador      |
| 0x2F | Nivel combustible        |

## Cristal del MCP2515

El codigo asume un cristal de **8 MHz**. Mira el componente metalico pequeño en el modulo MCP2515:

- Si dice `8.000` u `8M` → no cambiar nada
- Si dice `16.000` → cambiar en `include/config.h`:

```c
#define MCP2515_CRYSTAL MCP_16MHZ
```

## Notas sobre BMW F800GT

- El CAN bus opera a 500 kbps
- El soporte OBD-II en motos BMW puede ser parcial; no todos los PIDs estandar estan garantizados
- Si los PIDs estandar no responden, se puede hacer sniffing del bus CAN para identificar mensajes nativos de BMW
- El conector de diagnostico suele estar bajo el asiento o cerca del chasis

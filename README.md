# BMW F800GT CAN Bus Reader

Lector OBD-II para BMW F800GT usando ESP32 HW-395 CP2102 (38 pines) y transceiver SN65HVD230 con el controlador CAN nativo del ESP32 (TWAI).

## Hardware necesario

- ESP32 HW-395 CP2102 38 pines
- Modulo SN65HVD230 (transceiver CAN 3.3V)
- 4 cables dupont hembra-hembra (usar cables de buena calidad o soldar)
- Cable USB con datos (no solo de carga)

## Conexiones

### Tabla de cableado

| Pin ESP32 (etiqueta en placa) | Pin SN65HVD230 | Funcion    | Notas   |
|-------------------------------|-----------------|------------|---------|
| P4                            | CTX             | CAN TX     | Directo |
| P5                            | CRX             | CAN RX     | Directo |
| 3V3 (lado derecho)            | 3V3             | Alimentacion | Directo |
| GND                           | GND             | Tierra     | Directo |

Del SN65HVD230 a la moto:

| Pin SN65HVD230 | BMW F800GT (conector diag.) |
|-----------------|-----------------------------|
| CANH            | CAN-H                       |
| CANL            | CAN-L                       |

### Pinout ESP32 HW-395 (38 pines)

Los pines marcados con `►` son los que hay que conectar al SN65HVD230:

```
                    ┌───────────┐
              3V3   │          │  GND
               EN   │          │  P23
              SVP   │          │  P22
              SVN   │          │  TX
              P34   │          │  RX
              P35   │          │  P21
              P32   │          │  GND
              P33   │          │  P19
              P25   │  HW-395 │  P18
              P26   │          │  P5   ◄── CRX
              P27   │          │  P17
              P14   │          │  P16
              P12   │          │  P4   ◄── CTX
  GND ──►     GND   │          │  P0
              P13   │          │  P2
              SD2   │          │  P15
              SD3   │          │  SD1
              CMD   │          │  SD0
               5V   │          │  CLK
                    └────┤├────┘
                         USB
  3V3 ──► 3V3 (lado derecho, arriba)
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

La primera vez descargara el toolchain de ESP32 automaticamente.

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

Salida esperada con el SN65HVD230 conectado (sin la moto):

```
=== BMW F800GT CAN Bus Reader ===
CAN bus initialized (TWAI, 500 kbps) TX=4 RX=5
--- Reading OBD-II PIDs ---
PID 0x05: no response
PID 0x0C: no response
...
```

Los "no response" son normales sin la moto conectada. Al conectar CANH y CANL al conector de diagnostico de la BMW F800GT con el motor encendido, deberian aparecer los valores.

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

## Notas sobre BMW F800GT

- El CAN bus opera a 500 kbps
- El soporte OBD-II en motos BMW puede ser parcial; no todos los PIDs estandar estan garantizados
- Si los PIDs estandar no responden, se puede hacer sniffing del bus CAN para identificar mensajes nativos de BMW
- El conector de diagnostico suele estar bajo el asiento o cerca del chasis

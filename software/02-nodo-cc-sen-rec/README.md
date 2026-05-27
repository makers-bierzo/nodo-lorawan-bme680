# 02 - Nodo CubeCell: Envío y recepción LoRaWAN (downlink)

Este programa implementa un **nodo LoRaWAN completo** con la CubeCell. Realiza el Join a la red, envía datos de forma periódica (uplink) y es capaz de **recibir órdenes desde el servidor** (downlink). Es la base sobre la que se integrará la lectura del sensor BME680.

Está basado en el ejemplo oficial de Heltec [`LoRaWan_downlinkdatahandle`](https://github.com/HelTecAutomation/CubeCell-Arduino/blob/master/libraries/LoRa/examples/LoRaWAN/LoRaWan_downlinkdatahandle/LoRaWan_downlinkdatahandle.ino).

> 💡 Puedes encontrar más ejemplos oficiales de LoRaWAN para CubeCell en:
> **[github.com/HelTecAutomation/CubeCell-Arduino › libraries/LoRa/examples/LoRaWAN](https://github.com/HelTecAutomation/CubeCell-Arduino/tree/master/libraries/LoRa/examples/LoRaWAN)**

---

## ¿Qué hace el código?

### Máquina de estados (loop)

El `loop()` no ejecuta código lineal, sino que avanza por cinco estados de forma cíclica:

```
INIT → JOIN → SEND → CYCLE → SLEEP → (despierta) → SEND → ...
```

| Estado | Qué hace |
|---|---|
| `DEVICE_STATE_INIT` | Genera el DevEUI desde el Chip ID, imprime los parámetros y llama a `LoRaWAN.init()` |
| `DEVICE_STATE_JOIN` | Realiza el Join a la red (OTAA o ABP). Se queda aquí hasta conseguirlo |
| `DEVICE_STATE_SEND` | Llama a `prepareTxFrame()` para construir el payload y lo envía con `LoRaWAN.send()` |
| `DEVICE_STATE_CYCLE` | Calcula el tiempo hasta el próximo envío y programa el temporizador con `LoRaWAN.cycle()` |
| `DEVICE_STATE_SLEEP` | Pone el microcontrolador en modo de bajo consumo. El temporizador lo despierta cuando toca enviar |

### Funciones principales

**`prepareTxFrame(port)`**
Construye el payload uplink que se envía al servidor. Actualmente envía 4 bytes de ejemplo. Aquí es donde se añadirán las lecturas reales del BME680 (temperatura, humedad, presión y gas).

**`downLinkDataHandle(mcpsIndication)`**
Callback automático que se ejecuta cada vez que el servidor envía un mensaje al nodo (downlink). Imprime por Serial la ventana de recepción (RX1 o RX2), el tamaño y los bytes recibidos en hexadecimal. Como ejemplo, interpreta los 3 primeros bytes como un color RGB y enciende el LED durante 5 segundos.

```
+REV DATA:RXWIN1, RXSIZE 3, PORT 2
+REV DATA:FF0000    ← rojo
```

---

## Configuración antes de compilar

### 1. Credenciales OTAA

En `main.cpp`, sustituye los arrays de ceros por los valores de tu servidor LoRaWAN (TTN, ChirpStack…):

```cpp
uint8_t devEui[] = { 0x??, 0x??, ... };  // Generado automáticamente si LORAWAN_DEVEUI_AUTO=1
uint8_t appEui[] = { 0x??, 0x??, ... };  // JoinEUI de tu aplicación
uint8_t appKey[] = { 0x??, 0x??, ... };  // AppKey de tu dispositivo
```

> Con `LORAWAN_DEVEUI_AUTO=1` (activo por defecto en `platformio.ini`) el DevEUI se genera automáticamente a partir del Chip ID único de la CubeCell, por lo que no es necesario modificarlo.

### 2. Parámetros de `platformio.ini`

La plataforma `heltec-cubecell` tiene opciones nativas para LoRaWAN que evitan warnings de compilación. Se usan en lugar de `-D` en `build_flags`:

| Opción | Valor por defecto | Descripción |
|---|---|---|
| `lorawan.region` | `EU868` | Región de frecuencias (Europa 868 MHz) |
| `lorawan.netmode` | `OTAA` | Modo de activación: `OTAA` (recomendado) o `ABP` |
| `lorawan.adr` | `ON` | ADR: el servidor ajusta la tasa de datos automáticamente |
| `lorawan.uplinkmode` | `UNCONFIRMED` | `UNCONFIRMED` = sin ACK, `CONFIRMED` = con ACK |
| `lorawan.net_reserve` | `OFF` | `OFF` = hace Join de nuevo tras un reset |
| `lorawan.deveui` | `AUTO` | `AUTO` = genera el DevEUI desde el Chip ID |
| `lorawan.rgb` | `ACTIVE` | LED RGB habilitado para feedback visual |
| `lorawan.at_support` | `OFF` | Comandos AT por Serial (desactivado) |

### 3. Intervalo de envío

```cpp
uint32_t appTxDutyCycle = 15000;  // ms — cambia este valor (mínimo recomendado: 30000)
```

---

## Flujo de arranque (monitor serie)

Al encender el dispositivo verás algo similar a esto a 115200 baudios:

```
DevEUI : 1A-C8-F7-02-90-3D-5B-00
AppEUI : 00-00-00-00-00-00-00-00
AppKey : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
CLASS  : A
REGION : EU868

joining...
joined!
```

A partir de ahí, cada 15 segundos (configurable) se envía un uplink y el dispositivo vuelve a dormir.

---

## Hardware compatible

- Heltec CubeCell Board v2 (ASR6501)
- Heltec CubeCell Board Plus (ASR6601)

## Próximos pasos

- [ ] Integrar la lectura del sensor **BME680** en `prepareTxFrame()` y codificar temperatura, humedad, presión y gas en el payload uplink.
- [ ] Definir el formato del payload downlink para enviar configuraciones al nodo (intervalo de envío, umbrales de alerta, etc.).

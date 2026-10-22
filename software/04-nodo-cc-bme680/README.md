# 04 - Nodo CubeCell LoRaWAN con BME680

Ejemplo de nodo CubeCell que mide temperatura, humedad, presión y resistencia de gas con el BME680 y transmite esos valores por LoRaWAN OTAA. Sigue el ciclo de estados de los ejemplos Heltec. El sensor se alimenta desde Vext durante la medición y se apaga antes del periodo de sueño.

No calcula IAQ: la lectura de resistencia de gas no es por sí sola un índice de calidad del aire.

## Hardware y conexiones

- Heltec CubeCell Board compatible con `cubecell_board_v2`.
- Módulo BME680 conectado a I²C.
- `Vext` de CubeCell a VCC del módulo; GND común; SDA y SCL a las líneas I²C.

La clase `BME680_Class` de Heltec localiza automáticamente el dispositivo en `0x76` o `0x77`. Vext es activo en bajo y se apaga durante el ciclo LoRaWAN; por eso el VCC del sensor debe conectarse a Vext para aprovechar el ahorro de energía.

## Configuración OTAA

En `src/main.cpp`, sustituye `appEui` (JoinEUI) y `appKey` por los valores de tu servidor. `devEui` se genera automáticamente desde el Chip ID (`lorawan.deveui = AUTO`). Registra en TTN el DevEUI en el formato LSB que has comprobado con tu CubeCell. No copies claves de ejemplo de otros sketches.

La región está configurada como **EU868**, clase A, ADR activo y uplinks no confirmados. El intervalo inicial es 90 segundos (`appTxDutyCycle`).

## Medición y uplink

Antes de cada uplink el programa:

1. Enciende Vext y espera a que se estabilice la alimentación.
2. Inicializa el BME680 y configura oversampling 16×, filtro IIR de 4 muestras y calentador a 320 °C durante 150 ms, como en el ejemplo de Heltec.
3. Descarta la primera lectura y usa la siguiente para preparar el payload.
4. Apaga Vext y transmite el payload de 10 bytes por el puerto de aplicación 2.

Si no se detecta el sensor, no se envía un payload inválido; el nodo espera al siguiente ciclo de transmisión.

### Formato del payload

Los campos son big-endian y aparecen en este orden:

| Bytes | Tipo | Conversión |
|---|---|---|
| 0–1 | `int16` | Temperatura en centésimas de °C |
| 2–3 | `uint16` | Humedad en centésimas de % |
| 4–5 | `uint16` | Presión en décimas de hPa |
| 6–9 | `uint32` | Valor raw de gas de `BME680_Class`; la referencia Heltec lo presenta como `gas / 100` mOhm |

En el payload formatter de TTN puedes usar este decoder:

```javascript
function decodeUplink(input) {
  const b = input.bytes;
  if (b.length !== 10) {
    return { errors: ["Se esperaban 10 bytes"] };
  }

  const u16 = (i) => b[i] * 256 + b[i + 1];
  let temperature = u16(0);
  if (temperature & 0x8000) temperature -= 0x10000;

  const gasRaw = b[6] * 16777216 + b[7] * 65536 + b[8] * 256 + b[9];

  return {
    data: {
      temperature_c: temperature / 100,
      humidity_percent: u16(2) / 100,
      pressure_hpa: u16(4) / 10,
      gas_raw: gasRaw,
      gas_milliohms: gasRaw / 100
    }
  };
}
```

## Compilar y comprobar

Abre esta carpeta como proyecto PlatformIO, compila y carga el firmware. Abre el monitor serie a **115200 baudios**. En los eventos de TTN, selecciona el puerto 2 y comprueba que el payload decodificado contiene las cuatro mediciones.

## Referencia y licencia

La máquina de estados y la gestión del sensor siguen los ejemplos oficiales [`LoRaWan_MultiSensor`](https://github.com/HelTecAutomation/CubeCell-Arduino/tree/master/libraries/LoRa/examples/LoRaWAN/LoRaWAN_Sensors/LoRaWan_MultiSensor) y [`BME680_test.ino`](https://github.com/HelTecAutomation/CubeCell-Arduino/blob/master/libraries/Sensor_ThirdParty/examples/BME680/BME680_test/BME680_test.ino). `BME680_Class` forma parte de `Sensor_ThirdParty` del framework Heltec y declara licencia GPL-3.0; la dependencia conserva su licencia propia, distinta de la licencia MIT del código original del proyecto.

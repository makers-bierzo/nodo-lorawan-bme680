# 01 - Obtener DevEUI de la CubeCell

Este programa es el **paso 0** antes de programar el nodo LoRaWAN con el sensor BME680. Su única función es leer el DevEUI único del chip y mostrarlo por el puerto serie.

## ¿Qué es el DevEUI y por qué es importante?

El **DevEUI** (Device Extended Unique Identifier) es un identificador de 8 bytes grabado de fábrica en el chip de la CubeCell. Es único para cada dispositivo, de forma similar a una dirección MAC en Ethernet.

LoRaWAN lo usa para identificar unívocamente al dispositivo en la red. Si dos dispositivos tuviesen el mismo DevEUI **no podrían coexistir en la misma red**.

## Integración con TTN (The Things Network)

Para registrar el dispositivo en TTN con activación **OTAA** (la más común y recomendada) necesitas tres datos:

| Dato | Longitud | Quién lo genera | Para qué sirve |
|---|---|---|---|
| **DevEUI** | 8 bytes | El chip (único de fábrica) | Identifica unívocamente al dispositivo |
| **AppEUI** / JoinEUI | 8 bytes | Tú en TTN | Identifica la aplicación |
| **AppKey** | 16 bytes | Tú en TTN | Clave de cifrado para el Join |

### Flujo de registro

1. **Ejecuta este programa** y abre el monitor serie a 115200 baudios. Verás algo así:

    ```
    =================================
      CubeCell - Get Unique DevEUI
    =================================

    Chip Unique ID: 0x02F7C81A005B3D90

    DevEUI (LSB): 00-5B-3D-90-02-F7-C8-1A

    DevEUI (MSB - for LoRaWAN): 1A-C8-F7-02-90-3D-5B-00

    Use the MSB format (second one) for your LoRaWAN server.
    =================================
    ```

2. **Anota el DevEUI en formato MSB** (la segunda línea). Es el que usa TTN.

3. **En TTN** ve a *Applications → tu app → End devices → Register end device* e introduce el DevEUI. TTN te generará el AppEUI y el AppKey.

4. **En el programa definitivo** (con el sensor BME680) usa esos tres valores para hacer el Join a la red LoRaWAN.

## Cómo funciona `generateDeveuiByChipID()`

El chip almacena su identificador único en dos valores de 32 bits (`uint32_t`). La función los toma y construye el DevEUI de 8 bytes concatenando primero los bytes del segundo valor y luego los del primero:

```
Chip Unique ID:
  uniqueId[0] = 0x02F7C81A  →  bytes: 02  F7  C8  1A
  uniqueId[1] = 0x005B3D90  →  bytes: 00  5B  3D  90

DevEUI (LSB) = uniqueId[1] + uniqueId[0]:
  00  5B  3D  90  02  F7  C8  1A

DevEUI (MSB) = LSB invertido:
  1A  C8  F7  02  90  3D  5B  00
```

En resumen: el DevEUI no es más que los 64 bits del Chip Unique ID reordenados, lo que garantiza que sea único sin necesidad de ningún registro externo.

## Formato LSB vs MSB

La salida muestra el DevEUI en dos formatos porque distintos servidores LoRaWAN usan convenciones distintas:

- **LSB** (Least Significant Byte first): algunos entornos como Arduino lo esperan así internamente.
- **MSB** (Most Significant Byte first): es el formato estándar LoRaWAN que usa TTN y ChirpStack.

## Hardware compatible

- Heltec CubeCell Board (ASR650x)
- Heltec CubeCell Board Plus (ASR6601)

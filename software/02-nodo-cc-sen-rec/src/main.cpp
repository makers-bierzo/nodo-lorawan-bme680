#include "LoRaWan_APP.h"
#include "Arduino.h"

/*
 * Parámetros OTAA
 * ---------------
 * Sustituye estos valores por los de tu servidor LoRaWAN (TTN, ChirpStack, etc.)
 * DevEUI: si LORAWAN_DEVEUI_AUTO=1 se genera automáticamente desde el Chip ID.
 */
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*
 * Parámetros ABP (solo si overTheAirActivation = false)
 */
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe,
                      0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf,
                      0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67 };
uint32_t devAddr = (uint32_t)0x007e6ae1;

/* Máscara de canales: 0x00FF habilita los 8 primeros canales (EU868) */
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/* Configuración general de LoRaWAN — valores desde build_flags en platformio.ini */
LoRaMacRegion_t loraWanRegion   = ACTIVE_REGION;
DeviceClass_t   loraWanClass    = LORAWAN_CLASS;
uint32_t        appTxDutyCycle  = 15000;           // ms entre envíos
bool overTheAirActivation       = LORAWAN_NETMODE; // true = OTAA, false = ABP
bool loraWanAdr                 = LORAWAN_ADR;
bool keepNet                    = LORAWAN_NET_RESERVE;
bool isTxConfirmed              = LORAWAN_UPLINKMODE;
uint8_t appPort                 = 2;
uint8_t confirmedNbTrials       = 4;

/* ------------------------------------------------------------------
 * prepareTxFrame()
 * Construye el payload uplink que se enviará al servidor LoRaWAN.
 * TODO: reemplazar los bytes de ejemplo por lecturas reales del BME680.
 * ------------------------------------------------------------------ */
static void prepareTxFrame(uint8_t port) {
    appDataSize = 4;
    appData[0] = 0x00;  // Byte 0 — temperatura (placeholder)
    appData[1] = 0x01;  // Byte 1 — humedad    (placeholder)
    appData[2] = 0x02;  // Byte 2 — presión    (placeholder)
    appData[3] = 0x03;  // Byte 3 — gas        (placeholder)
}

/* ------------------------------------------------------------------
 * downLinkDataHandle()
 * Callback invocado automáticamente cuando se recibe un downlink
 * desde el servidor LoRaWAN (ventanas RX1 / RX2).
 * ------------------------------------------------------------------ */
void downLinkDataHandle(McpsIndication_t *mcpsIndication) {
    Serial.printf("+REV DATA:%s, RXSIZE %d, PORT %d\r\n",
                  mcpsIndication->RxSlot ? "RXWIN2" : "RXWIN1",
                  mcpsIndication->BufferSize,
                  mcpsIndication->Port);

    Serial.print("+REV DATA:");
    for (uint8_t i = 0; i < mcpsIndication->BufferSize; i++) {
        Serial.printf("%02X", mcpsIndication->Buffer[i]);
    }
    Serial.println();

    /* Ejemplo: los primeros 3 bytes del downlink codifican un color RGB */
    uint32_t color = (uint32_t)mcpsIndication->Buffer[0] << 16
                   | (uint32_t)mcpsIndication->Buffer[1] << 8
                   |            mcpsIndication->Buffer[2];

#if (LoraWan_RGB == 1)
    turnOnRGB(color, 5000);
    turnOffRGB();
#endif
}

/* ------------------------------------------------------------------
 * setup()
 * ------------------------------------------------------------------ */
void setup() {
    Serial.begin(115200);

#if (AT_SUPPORT)
    enableAt();
#endif

    deviceState = DEVICE_STATE_INIT;
    LoRaWAN.ifskipjoin();
}

/* ------------------------------------------------------------------
 * loop() — máquina de estados LoRaWAN
 * ------------------------------------------------------------------ */
void loop() {
    switch (deviceState) {
        case DEVICE_STATE_INIT: {
#if (LORAWAN_DEVEUI_AUTO)
            LoRaWAN.generateDeveuiByChipID();
#endif
            printDevParam();
            LoRaWAN.init(loraWanClass, loraWanRegion);
            deviceState = DEVICE_STATE_JOIN;
            break;
        }
        case DEVICE_STATE_JOIN: {
            LoRaWAN.join();
            break;
        }
        case DEVICE_STATE_SEND: {
            prepareTxFrame(appPort);
            LoRaWAN.send();
            deviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE: {
            txDutyCycleTime = appTxDutyCycle + randr(0, APP_TX_DUTYCYCLE_RND);
            LoRaWAN.cycle(txDutyCycleTime);
            deviceState = DEVICE_STATE_SLEEP;
            break;
        }
        case DEVICE_STATE_SLEEP: {
            LoRaWAN.sleep();
            break;
        }
        default: {
            deviceState = DEVICE_STATE_INIT;
            break;
        }
    }
}

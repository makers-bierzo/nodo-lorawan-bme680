#include <Arduino.h>
#include <LoRaWan_APP.h>
#include <BME680.h>
#include <math.h>

/* Credenciales OTAA: sustituye JoinEUI y AppKey por las de tu servidor. */
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Símbolos requeridos por el framework aunque este ejemplo use OTAA. */
uint8_t nwkSKey[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
uint8_t appSKey[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
uint32_t devAddr = 0x00000000;

uint16_t userChannelsMask[6] = {
  0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t loraWanClass = LORAWAN_CLASS;
uint32_t appTxDutyCycle = 90000;
bool overTheAirActivation = LORAWAN_NETMODE;
bool loraWanAdr = LORAWAN_ADR;
bool keepNet = LORAWAN_NET_RESERVE;
bool isTxConfirmed = LORAWAN_UPLINKMODE;
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;

namespace {

constexpr uint32_t SENSOR_POWER_SETTLE_MS = 500;
constexpr uint32_t SENSOR_STARTUP_MS = 1000;
constexpr uint32_t SENSOR_FIRST_SAMPLE_WAIT_MS = 500;
constexpr float SEA_LEVEL_PRESSURE_HPA = 1013.25F;

BME680_Class bme680;

void powerSensor(bool enabled) {
  pinMode(Vext, OUTPUT);
  // Vext es activo en bajo.
  digitalWrite(Vext, enabled ? LOW : HIGH);
}

void configureBme680() {
  bme680.setOversampling(TemperatureSensor, Oversample16);
  bme680.setOversampling(HumiditySensor, Oversample16);
  bme680.setOversampling(PressureSensor, Oversample16);
  bme680.setIIRFilter(IIR4);
  bme680.setGas(320, 150);
}

bool readBme680(int32_t &temperatureRaw,
                int32_t &humidityRaw,
                int32_t &pressureRaw,
                int32_t &gasRaw) {
  powerSensor(true);
  delay(SENSOR_POWER_SETTLE_MS);

  if (!bme680.begin(I2C_STANDARD_MODE)) {
    Serial.println("ERROR: no se encuentra el BME680 en 0x76 ni 0x77.");
    powerSensor(false);
    return false;
  }

  delay(SENSOR_STARTUP_MS);
  configureBme680();

  // La primera lectura inicia/estabiliza el ciclo; la segunda es la enviada.
  int32_t discardedTemperature;
  int32_t discardedHumidity;
  int32_t discardedPressure;
  int32_t discardedGas;
  bme680.getSensorData(discardedTemperature, discardedHumidity,
                       discardedPressure, discardedGas);
  delay(SENSOR_FIRST_SAMPLE_WAIT_MS);
  bme680.getSensorData(temperatureRaw, humidityRaw, pressureRaw, gasRaw);

  powerSensor(false);
  return true;
}

void writeUint16BigEndian(uint8_t index, uint16_t value) {
  appData[index] = static_cast<uint8_t>(value >> 8);
  appData[index + 1] = static_cast<uint8_t>(value);
}

void writeUint32BigEndian(uint8_t index, uint32_t value) {
  appData[index] = static_cast<uint8_t>(value >> 24);
  appData[index + 1] = static_cast<uint8_t>(value >> 16);
  appData[index + 2] = static_cast<uint8_t>(value >> 8);
  appData[index + 3] = static_cast<uint8_t>(value);
}

void printReading(int32_t temperatureRaw,
                  int32_t humidityRaw,
                  int32_t pressureRaw,
                  int32_t gasRaw) {
  const float temperatureC = temperatureRaw / 100.0F;
  const float humidityPercent = humidityRaw / 1000.0F;
  const float pressureHpa = pressureRaw / 100.0F;
  const float gasMilliOhms = gasRaw / 100.0F;
  const float altitudeMeters =
      44330.0F * (1.0F - powf(pressureHpa / SEA_LEVEL_PRESSURE_HPA, 0.1903F));

  Serial.print("BME680: T=");
  Serial.print(temperatureC, 2);
  Serial.print(" C, RH=");
  Serial.print(humidityPercent, 2);
  Serial.print(" %, P=");
  Serial.print(pressureHpa, 2);
  Serial.print(" hPa, alt~");
  Serial.print(altitudeMeters, 1);
  Serial.print(" m, gas=");
  Serial.print(gasMilliOhms, 2);
  Serial.println(" mOhm");
}

bool prepareTxFrame(uint8_t port) {
  (void)port;

  int32_t temperatureRaw;
  int32_t humidityRaw;
  int32_t pressureRaw;
  int32_t gasRaw;

  appDataSize = 0;
  if (!readBme680(temperatureRaw, humidityRaw, pressureRaw, gasRaw)) {
    return false;
  }

  printReading(temperatureRaw, humidityRaw, pressureRaw, gasRaw);

  // Payload de 10 bytes, big-endian:
  // T: int16 en 0,01 °C; HR: uint16 en 0,01 %;
  // P: uint16 en 0,1 hPa; gas: uint32 en escala raw de BME680_Class.
  const int16_t temperatureCentiC = static_cast<int16_t>(temperatureRaw);
  const uint16_t temperatureEncoded = static_cast<uint16_t>(temperatureCentiC);
  const uint16_t humidityCentiPercent =
      static_cast<uint16_t>((humidityRaw + 5) / 10);
  const uint16_t pressureDeciHpa =
      static_cast<uint16_t>((pressureRaw + 5) / 10);
  const uint32_t gasEncoded = static_cast<uint32_t>(gasRaw);

  writeUint16BigEndian(0, temperatureEncoded);
  writeUint16BigEndian(2, humidityCentiPercent);
  writeUint16BigEndian(4, pressureDeciHpa);
  writeUint32BigEndian(6, gasEncoded);
  appDataSize = 10;
  return true;
}

}  // namespace

void setup() {
  Serial.begin(115200);
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();
}

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
      if (prepareTxFrame(appPort)) {
        LoRaWAN.send();
      }
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

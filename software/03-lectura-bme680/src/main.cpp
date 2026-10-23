#include <Arduino.h>
#include <BME680.h>
#include <CubeCell_NeoPixel.h>
#include <math.h>

namespace {

constexpr uint32_t SERIAL_SPEED = 115200;
constexpr unsigned long READ_INTERVAL_MS = 5000;
constexpr float SEA_LEVEL_PRESSURE_HPA = 1029.0F;
constexpr uint8_t RGB_BRIGHTNESS = 32;

BME680_Class bme680;
CubeCell_NeoPixel pixels(1, RGB, NEO_GRB + NEO_KHZ800);
uint8_t nextRgbColor = 0;

void showNextRgbColor() {
  switch (nextRgbColor) {
    case 0:
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      break;
    case 1:
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      break;
    default:
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      break;
  }

  pixels.show();
  nextRgbColor = (nextRgbColor + 1) % 3;
}

void configureBme680() {
  bme680.setOversampling(TemperatureSensor, Oversample16);
  bme680.setOversampling(HumiditySensor, Oversample16);
  bme680.setOversampling(PressureSensor, Oversample16);
  bme680.setIIRFilter(IIR4);
  bme680.setGas(320, 150);
}

float calculateAltitudeMeters(float pressureHpa) {
  return 44330.0F * (1.0F - powf(pressureHpa / SEA_LEVEL_PRESSURE_HPA, 0.1903F));
}

void printReading() {
  int32_t temperatureRaw;
  int32_t humidityRaw;
  int32_t pressureRaw;
  int32_t gasRaw;

  bme680.getSensorData(temperatureRaw, humidityRaw, pressureRaw, gasRaw);

  const float temperatureC = temperatureRaw / 100.0F;
  const float humidityPercent = humidityRaw / 1000.0F;
  const float pressureHpa = pressureRaw / 100.0F;
  const float gasMilliOhms = gasRaw / 100.0F;

  Serial.println("------------------------------");
  Serial.print("Temperatura: ");
  Serial.print(temperatureC, 2);
  Serial.println(" °C");

  Serial.print("Humedad:     ");
  Serial.print(humidityPercent, 2);
  Serial.println(" %");

  Serial.print("Presion:     ");
  Serial.print(pressureHpa, 2);
  Serial.println(" hPa");

  Serial.print("Altitud aprox.: ");
  Serial.print(calculateAltitudeMeters(pressureHpa), 2);
  Serial.println(" m (nivel del mar: 1029.0 hPa)");

  Serial.print("Resistencia de gas: ");
  Serial.print(gasMilliOhms, 2);
  Serial.println(" mOhm");
}

}  // namespace

void setup() {
  Serial.begin(SERIAL_SPEED);

  // Vext es activo en bajo. Conecta VCC del modulo del sensor a Vext.
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(500);

  pixels.begin();
  pixels.setBrightness(RGB_BRIGHTNESS);
  pixels.clear();
  pixels.show();

  Serial.println();
  Serial.println("CubeCell - Lectura del BME680 (Heltec)");

  while (!bme680.begin(I2C_STANDARD_MODE)) {
    Serial.println("No se encuentra el BME680; nuevo intento en 3 segundos.");
    delay(3000);
  }

  configureBme680();
  Serial.println("BME680 inicializado. Lecturas cada 5 segundos; RGB rojo, verde y azul.");
}

void loop() {
  printReading();
  showNextRgbColor();
  delay(READ_INTERVAL_MS);
}

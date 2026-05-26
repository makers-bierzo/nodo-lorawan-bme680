#include <Arduino.h>
#include <LoRaWan_APP.h>

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=================================");
  Serial.println("  CubeCell - Get Unique DevEUI");
  Serial.println("=================================");
  Serial.println();
  
  LoRaWAN.generateDeveuiByChipID();
  
  Serial.print("Chip Unique ID: ");
  #if defined(__asr6601__)
    uint32_t uniqueId[2];
    system_get_chip_id(uniqueId);
  #else
    uint32_t uniqueId[2];
    CyGetUniqueId(uniqueId);
  #endif
  Serial.print("0x");
  // Serial.print(uniqueId[0], HEX);
  // Serial.print(uniqueId[1], HEX);
  Serial.printf("%08X%08X", uniqueId[0], uniqueId[1]);
  Serial.println();
  Serial.println();
  
  Serial.print("DevEUI (LSB): ");
  for(int i = 0; i < 8; i++)
  {
    Serial.printf("%02X", devEui[i]);
    if(i < 7) Serial.print("-");
  }
  Serial.println();
  Serial.println();
  
  Serial.print("DevEUI (MSB - for LoRaWAN): ");
  for(int i = 7; i >= 0; i--)
  {
    Serial.printf("%02X", devEui[i]);
    if(i > 0) Serial.print("-");
  }
  Serial.println();
  Serial.println();
  
  Serial.println("Use the MSB format (second one) for your LoRaWAN server.");
  Serial.println("=================================");
}

void loop() {
  delay(10000);
  
  Serial.println("DevEUI (MSB): ");
  for(int i = 7; i >= 0; i--)
  {
    Serial.printf("%02X", devEui[i]);
    if(i > 0) Serial.print("-");
  }
  Serial.println();
}

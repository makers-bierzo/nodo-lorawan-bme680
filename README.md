# 🌿 Nodo LoRaWAN Ambiental — CubeCell + BME680

**Makers Bierzo** · El Bierzo, León

Nodo IoT de monitorización ambiental de bajo consumo basado en hardware propio, protocolo LoRaWAN y la red The Things Network (TTN). Desarrollado como proyecto abierto de la asociación [Makers Bierzo](https://github.com/makers-bierzo).

---

## ¿Qué mide?

| Parámetro | Sensor | Unidad |
|---|---|---|
| Temperatura | BME680 | °C |
| Humedad relativa | BME680 | % |
| Presión atmosférica | BME680 | hPa |
| Calidad del aire (VOC/IAQ) | BME680 | índice IAQ |

---

## Hardware

- **MCU + Radio:** Heltec CubeCell HTCC-AB01 (ASR6501, LoRa 868 MHz integrado)
- **Sensor:** Bosch BME680 (I²C)
- **PCB:** Diseño propio (archivos en `/hardware`)
- **Alimentación:** Batería LiPo + panel solar opcional
- **Modo de operación:** LoRaWAN Class A con deep sleep entre transmisiones

## Contribuir

Las contribuciones son bienvenidas. Abre un issue o un pull request. Si eres de la zona y quieres unirte a la asociación, pásate por nuestros encuentros en El Bierzo.

---

## Licencia

MIT License © 2025 Makers Bierzo

---

*Proyecto desarrollado con 💚 en El Bierzo*

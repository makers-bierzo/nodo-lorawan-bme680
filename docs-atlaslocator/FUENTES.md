# Fuentes consultadas de AtlasLocator

Consulta realizada el **19 de noviembre de 2026**. Los extractos siguientes resumen la documentación pública relevante para integrar el nodo Makers Bierzo; las páginas pueden cambiar y varias guías están fechadas en 2022–2023.

## Conexión TTS / TTN / TTI

- [Ver más información de la integración de conexión TTS (TTN & TTI)](https://atlaslocator.com/es/ver-mas-informacion-de-la-integracion-de-conexion-ttn/) — artículo publicado el 14 de diciembre de 2022.
  - Describe conexión con The Things Network / The Things Stack e Industrial mediante **MQTT**.
  - Indica que el dispositivo debe estar dado de alta y funcionando antes de conectar AtlasLocator.
  - Instruye abrir la aplicación en TTS, entrar en **Integrations → MQTT** y generar una API key.
  - Enumera cuatro datos para dar de alta la conexión en AtlasLocator: **Username**, **Password (API key)**, **Application ID** y **Device ID**.
  - Precisa dónde localizar cada uno: Username y la generación de API key en la pantalla MQTT; Application ID en la selección/listado de aplicaciones; Device ID en **End devices**. La guía no proporciona valores específicos del proyecto.
  - La configuración de AtlasLocator se hace desde **Mis Dispositivos**, escogiendo primero protocolo y zona geográfica, y luego introduciendo los valores.
  - Recomienda dar de alta el modelo de dispositivo en TTS seleccionando marca/modelo; si se da de alta manualmente, hay que definir un script/formatter para decodificar el uplink payload.
  - No especifica en el artículo host/broker, puerto, topic MQTT ni permisos exactos de la API key. No inferir esos valores a partir de la guía.

## Alta y gestión de dispositivos

- [Mis Dispositivos](https://atlaslocator.com/es/mis-dispositivos/) — artículo publicado el 16 de diciembre de 2022.
  - Un dispositivo debe estar en una red (TTS/TTN/TTI, Helium, etc.) y asociarse a un monitorizado; sin ello no se explotan los datos.
  - Al crear el dispositivo, se define nombre, marca y modelo. Existe la marca **CUSTOM** para un dispositivo propio.
  - Luego se eligen tecnología, operador y protocolo de integración.
  - La sección de etiquetado muestra datos recibidos, valor y fecha, y permite definir naturaleza y nombre. Si no hay datos, la sección aparece vacía.
  - Desde la alarma del dispositivo se configura la falta de datos; las alarmas según valores recibidos se configuran en monitorizados.
  - El artículo remite a la guía TTS/TTN/TTI para los parámetros de conexión.
- [Mis Monitorizados](https://atlaslocator.com/es/mis-monitorizados/) — artículo publicado el 16 de diciembre de 2022.
  - Un monitorizado representa la persona, lugar u objeto al que se asocian uno o varios dispositivos.
  - Se crea desde la pantalla principal con **+** y permite definir nombre, tipo, ubicación, imagen, dispositivos y notas.
  - Para un sensor fijo se puede seleccionar tipo **Lugar** y establecer la ubicación en el mapa. La guía advierte que no se debe fijar una ubicación manual si el dispositivo proporciona GPS.
  - Para asociar un dispositivo, se pulsa **+** en el campo **Dispositivos**; después se guarda el monitorizado.
  - Los datos asociados se consultan en mapas, gráficos y tablas; las alarmas basadas en mediciones se configuran en el monitorizado.
  - Para una alarma se definen nombre, disparador, valor y dimensión; también se puede retrasar por número de datos recibidos o por minutos produciéndose, y establecer canal, destinatarios e intervalo de notificaciones.

## Gráficos

- [Gráficas](https://atlaslocator.com/es/graficas/) — artículo publicado el 20 de enero de 2023.
  - Permite seleccionar el monitorizado, una o varias dimensiones, tipo de gráfico, rango de fechas o de datos y frecuencia de refresco.
  - Permite descargar en Excel los datos representados.

## Comunicaciones y dispositivos

- [Comunicaciones](https://atlaslocator.com/es/comunicaciones/) — indica soporte de operadores LoRaWAN TTS (TTN & TTI) y Helium. La ruta TTS usa MQTT; Helium usa una integración HTTP distinta. Menciona también cobertura y mapas de antenas.
- [Dispositivos soportados](https://atlaslocator.com/es/dispositivos_codigo/) — afirma compatibilidad con LoRaWAN y permite filtrar modelos. La página presenta un RAK7204 con BME680, pero esto **no demuestra** que el firmware/custom device de Makers Bierzo esté validado. Indica contactar a AtlasLocator si el dispositivo no está en el listado o no funciona.

## Funciones de la aplicación

- [Aplicación IoT](https://atlaslocator.com/es/aplicacion-iot/) — describe centralización y compartición de dispositivos, visualización en mapa, gráficos, tablas, exportación a Excel, alarmas y notificaciones.
- [Guía General](https://atlaslocator.com/general_guide_es) — índice a mapas, gráficos, tablas, Mis Dispositivos, Mis Monitorizados y TTS/TTN/TTI.
- [Inicio de AtlasLocator](https://atlaslocator.com/es/) — presenta AtlasLocator como plataforma de gestión IoT con mapas, gráficos, tablas, exportación y notificaciones.

## Referencia no aplicable: Helium

- [Integración Helium](https://atlaslocator.com/es/ver-mas-informacion-de-la-integracion-de-conexion-helium/) — indica que Helium se conecta por HTTP, con endpoint publicado `https://app.atlaslocator.com/api/v1/thirdparty/helium/v1`, y un encabezado `atl-key`. Esta ruta es distinta a TTS/TTN/TTI y **no se debe usar para este proyecto**. Aunque la página muestra un valor de ejemplo para la cabecera, no se copia a esta documentación ni debe considerarse una credencial del proyecto.

## Datos propios del nodo (fuente del repositorio)

- [`software/05-nodo-cc-bme680/README.md`](../software/05-nodo-cc-bme680/README.md)
- [`software/05-nodo-cc-bme680/src/main.cpp`](../software/05-nodo-cc-bme680/src/main.cpp)

El firmware del proyecto transmite 12 bytes por LoRaWAN en el puerto 2: temperatura, humedad relativa, presión, resistencia del gas y batería. El decoder necesario para TTS está documentado en `software/05-nodo-cc-bme680/README.md`, no se repite en la guía de AtlasLocator. El nodo **no calcula IAQ** y **no lleva GPS**.

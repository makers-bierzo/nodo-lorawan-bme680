# Integración del nodo con AtlasLocator

Guía paso a paso para conectar el nodo **CubeCell + BME680** con AtlasLocator mediante MQTT desde TTS.

El formatter/decodificador de uplink en TTS es necesario y está explicado en el [README del firmware](../software/05-nodo-cc-bme680/README.md). Aquí no se repite.

## Antes de empezar

### 1. Comprueba que el dispositivo está dado de alta y enviando uplinks en TTS.

### 2. Obtén los datos en TTS

Haz estos pasos en la **misma consola/instancia de TTS** donde está registrado el nodo. Los nombres concretos de algunos menús pueden variar entre TTN, TTS y TTI, pero AtlasLocator documenta estas ubicaciones:

1. **Application ID**
   - Abre **Applications** y localiza la aplicación que contiene este nodo.
   - Copia el valor identificado como **Application ID** en la lista o en la información de la aplicación.
   - Usa el identificador, no el nombre descriptivo que se muestra como título. No uses el JoinEUI/AppEUI del firmware.
2. **Device ID**
   - En la aplicación, abre **End devices** y selecciona el dispositivo CubeCell que envía los uplinks.
   - Copia el ID del dispositivo que aparece en su ficha (puede mostrarse como **End device ID**). No copies el DevEUI: es otro identificador.
3. **Abre la integración MQTT**
   - Dentro de la aplicación, abre **Integrations → MQTT**. Esta es la integración que utilizará AtlasLocator.
4. **Username MQTT**
   - Copia el valor que aparece como **Username** en esa pantalla. No es el usuario/correo con el que inicias sesión en TTS.
5. **API key MQTT (se introduce como Password en AtlasLocator)**
   - En la pantalla MQTT, pulsa **Generate new API Key**.
   - Si el formulario permite ponerle nombre, usa uno identificable, por ejemplo `AtlasLocator`.
   - Copia la clave generada y guárdala de forma segura: es la contraseña que se pegará en AtlasLocator. No es la AppKey OTAA del dispositivo. Trátala como secreta y no la incluyas en capturas, logs, código o documentos del repositorio.
   - Si la clave se pierde, genera otra y actualiza el valor en AtlasLocator. No generes claves repetidamente si ya tienes una válida.
Antes de salir de TTS, comprueba que el dispositivo elegido pertenece a la aplicación cuyo Application ID y datos MQTT has copiado. Mantén los identificadores juntos en una nota temporal segura, pero no guardes ahí la API key si la nota no está protegida.

## Dar de alta el dispositivo en AtlasLocator

1. **Crea el dispositivo.**
   - En [AtlasLocator](https://app.atlaslocator.com/), abre **Mis Dispositivos** y pulsa **+**.
   - Introduce un nombre y selecciona marca y modelo. Para este nodo propio, usa la marca **CUSTOM** y un modelo descriptivo, por ejemplo `Makers Bierzo — CubeCell BME680`.
   - Pulsa **Crear y continuar**.
2. **Configura la red y la integración.**
   - En la sección de comunicaciones del dispositivo, pulsa **+** y selecciona la tecnología **LoRaWAN** y el operador **TTS (TTN & TTI)**.
   - Selecciona la zona geográfica correspondiente a la configuración de red.
   - Configura el protocolo **MQTT** e introduce los cuatro valores obtenidos en **Antes de empezar**. No uses DevEUI, JoinEUI ni AppKey en esos campos.
   - Guarda la configuración.
3. **Comprueba que llegan los datos.**
   - Espera al siguiente uplink y revisa el dispositivo en AtlasLocator.
   - Si aparecen datos, define su naturaleza y personaliza sus nombres si hace falta.
4. **Crea el monitorizado y añade el dispositivo.**
   - En **Mis Monitorizados**, pulsa **+**.
   - Escribe un nombre, por ejemplo `Estación ambiental Makers Bierzo`. En **Tipo**, elige **Lugar** para una instalación fija.
   - Como este nodo no tiene GPS, fija su ubicación en el mapa si quieres verlo geolocalizado.
   - En **Dispositivos**, pulsa **+** y selecciona el CubeCell + BME680 que acabas de dar de alta.
   - Añade imagen o notas si te hacen falta y guarda.
5. **Consulta los gráficos y configura las alarmas.**
   - **Gráficos:** abre **Gráficas**, selecciona el monitorizado y elige una o varias dimensiones disponibles (por ejemplo, temperatura o humedad), el tipo de gráfico y el rango de fechas o de datos. Puedes ajustar el refresco automático y descargar los datos mostrados en Excel.
   - **Alarmas por medición:** en **Mis Monitorizados**, pulsa la campana del monitorizado y luego **+**. Asigna un nombre, selecciona la dimensión (por ejemplo, temperatura), el disparador (mayor que, menor que, igual o diferente) y el valor límite.
   - Configura cuándo debe dispararse (tras un número de lecturas o durante cierto tiempo), el canal de aviso —por ejemplo, correo electrónico—, destinatarios e intervalo entre notificaciones. Guarda y activa la alarma.
   - La alarma de la campana del **dispositivo** es distinta: sirve para avisar de que no llegan datos. Las alarmas por valores medidos se configuran en el monitorizado.

Si no aparecen datos, comprueba primero que llegan uplinks a TTS, que el formatter está configurado y que los cuatro valores MQTT son correctos. Si el modelo CUSTOM no funciona, consulta con [AtlasLocator](https://atlaslocator.com/es/contacto/).

## Fuentes

Las páginas y los hechos extraídos están resumidos en [FUENTES.md](FUENTES.md), con enlaces oficiales y notas sobre lo que la documentación pública no especifica.

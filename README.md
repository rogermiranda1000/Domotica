# Domótica
Crea tu propia casa domótica mediante Arduino y Raspberry

## SETUP
- Necesitas poder [cargar código en la placa Lolin NodeMCU](https://www.instructables.com/Get-Started-With-ESP8266-NodeMCU-Lolin-V3/)
- Para la comunicación MQTT, arduino necesita [Libreria PubSubClient](https://pubsubclient.knolleary.net/)
- Para la recepción de MQTT, el módulo central necesita [Mosquitto](https://mosquitto.org/blog/2013/01/mosquitto-debian-repository/). En '/etc/mosquitto/conf.d/mosquitto.conf' hay que indicarle la IP estática ('listener 1883 192.168.1.229' y 'allow_anonymous true').
- Se necesita MariaDB (el archivo a ejecutar para cargar la base de datos se encuentra en el [Módulo central](https://github.com/rogermiranda1000/Domotica#m%C3%B3dulo-central)). En su fichero de configuración hay que activar los schedulers (en linux, '/etc/mysql/my.cnf' y poner '[mariadb] event_scheduler=ON').
- El módulo central necesita una IP estática (busca como configurarlo dependiendo de su sistema)
- En la carpeta [libraries](https://github.com/rogermiranda1000/Domotica/tree/master/libraries) tienes las librerias que has de importar a Arduino

## Módulo botón
Envia una señal al pulsar el botón.
Para configurar su acción se ha de ir a Editar>Añadir botón, el nombre ha de ser su ID y el comando la acción que se desea realizar.

## Módulo seguridad
Detecta gases tóxicos para humanos. Avisa tanto por el buzzer, incluido en el módulo, como a la central.

## Módulo climático simple
Envia información sobre la temperatura, humedad y luz.
Para el uso en exteriores se recomienda el [Módulo climático avanzado](https://github.com/rogermiranda1000/Domotica#m%C3%B3dulo-clim%C3%A1tico-avanzado).

## Módulo climático avanzado
Envia información sobre la temperatura, humedad, nivel de agua y luz.

## Módulo estación meteorológica
Envia información sobre la temperatura, humedad, nivel de agua, luz, velocidad y dirección del viento.

Requerimientos:
- [Arduino UNO Wifi Rev2](https://store.arduino.cc/arduino-uno-wifi-rev2)
- [Sparkfun Weather Meter Kit](https://www.sparkfun.com/products/15901)
- [SparkFun Weather Shield](https://www.sparkfun.com/products/13956)
- [RJ11 Connector (x2)](https://www.sparkfun.com/products/132)

> **/!\\ Al soldar los pines del Shield no conecteis SDA ni SCL. /!\\**<br/>
> El Shield está creado para Arduino UNO, y la placa Arduino UNO Wifi Rev2 tiene un pinout de comunicación distinta. Además, [uno de los sensores tiene la misma ID de I2C que el chip de criptografia de la placa Rev2](https://forum.arduino.cc/t/arduino-uno-wifi-rev-2-and-liquidcrystal_i2c-solved/562494/18#msg4005531).<br/>
> El código se ha adaptado para usar los pines A4 y A5 como pines de I2C (mediante la libreria de SoftWire) para evitar los problemas mencionados.<br/>

> **/!\\ Si no usais Vin teneis que conectar Vin a +5V /!\\**<br/>
> El Shield está creado para Arduino UNO, y la placa [Arduino UNO Wifi Rev2 no suministra corriente a Vin si se alimenta mediante la toma de +5V](https://forum.arduino.cc/t/i2c-problem-while-using-sparkfun-weather-shield-on-arduino-uno-wifi-rev2/698082/33?u=rogermiranda1000).<br/>
> Un ejemplo de no usar Vin es cuando conectais la placa mediante USB.<br/>
> **Es importante que, si decidis usar Vin después de realizar la conexión, la quiteis o destruireis la placa.**

## Módulo alarma
Activa/desactiva la alarma. Se puede realizar mediante el keypad o el sensor dactilar.

## Módulo movimiento
Detecta el movimiento de la zona y avisa si detecta movimiento.

## Módulo apertura
Detecta aperturas de puertas y ventanas y avisa.

## Módulo enchufe
Da luz, o la bloquea en función de su estado.

## Módulo riego
Permite regar las plantas automáticamente.<br/>
Además de asignar la hora diaria de riego (o hacerlo manualmente), el programa central comprueba si hay altas probabilidades de lluvia al día siguiente y la humedad actual de las plantas para optimizar el agua.

## Módulo LED
Produce iluminación de distintos colores.

## Módulo central
El módulo central es el “cerebro” de todo el sistema domótico, además de realizar otras tareas.<br/>
Se compone de una parte que controla la comunicación (MQTT, mediante Python), otra responsable de la WEB (HTML, CSS, JavaScript y PHP), y otra de la base de datos (MariaDB).

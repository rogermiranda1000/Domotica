# Domótica
Crea tu propia casa domótica mediante Arduino y Raspberry

## Módulo botón
Envia una señal al pulsar el botón.

## Módulo seguridad
Detecta gases tóxicos para humanos. Avisa tanto por el buzzer, incluido en el módulo, como a la central.

## Módulo climático simple
Envia información sobre la temperatura, humedad y luz.
Para el uso en exteriores se recomiendo al Módulo climático avanzado.

## Módulo climático avanzado
Envia información sobre la temperatura, humedad, nivel de agua y luz.

## Módulo estación meteorológica
Envia información sobre la temperatura, humedad, nivel de agua, luz, velocidad y dirección del viento.

Requerimientos:
- [Sparkfun Weather Meter Kit](https://www.sparkfun.com/products/15901)
- [SparkFun Weather Shield](https://www.sparkfun.com/products/13956)
- [RJ11 Connector; x2][https://www.sparkfun.com/products/132]

## Módulo alarma
Activa/desactiva la alarma. Se puede realizar mediante el keypad o el sensor dactilar.

## Módulo movimiento
Detecta el movimiento de la zona y avisa si detecta movimiento.

## Módulo apertura
Detecta aperturas de puertas y ventanas y avisa.

## Módulo enchufe
Da luz, o la bloquea en función de su estado.

## Módulo riego
Permite regar las plantas automáticamente.
Además de asignar la hora diaria de riego (o hacerlo manualmente), el programa central comprueba si hay altas probabilidades de lluvia al día siguiente y la humedad actual de las plantas para optimizar el agua.

## Módulo LED
Produce iluminación de distintos colores.

## Módulo central
El módulo central es el “cerebro” de todo el sistema domótico, además de realizar otras tareas.
Se compone de una parte que controla la comunicación (MQTT, mediante Python), otra responsable de la WEB (HTML, CSS, JavaScript y PHP), y otra de la base de datos (MariaDB).

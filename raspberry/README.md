# Módulo central
El módulo central es el “cerebro” de todo el sistema domótico, además de realizar otras tareas.<br/>
Se compone de una parte que controla la comunicación (MQTT, mediante Python), otra responsable de la WEB (HTML, CSS, JavaScript y PHP), y otra de la base de datos (MariaDB).

## SETUP
- Para la recepción de MQTT, el módulo central necesita [Mosquitto](https://mosquitto.org/blog/2013/01/mosquitto-debian-repository/). En '/etc/mosquitto/conf.d/mosquitto.conf' hay que indicarle la IP estática ('listener 1883 192.168.1.229' y 'allow_anonymous true').
- Se necesita MariaDB (el archivo a ejecutar para cargar la base de datos se encuentra en el [Módulo central](https://github.com/rogermiranda1000/Domotica#m%C3%B3dulo-central)). En su fichero de configuración hay que activar los schedulers (en linux, '/etc/mysql/my.cnf' y poner '[mariadb] event_scheduler=ON').
- El módulo central necesita una IP estática (busca como configurarlo dependiendo de su sistema)

## Ejecutar automáticamente el .py en Ubuntu
- Copiar 'mqtt_seguimiento_modulos.service' a la carpeta '/etc/systemd/system/'. Cambie la ruta de 'ExecStart' dependiendo de donde tenga el .py (para ver el directorio absouluto ejecutar 'pwd').<br/>
- Ejecutar 'sudo chmod 644 /etc/systemd/system/mqtt_seguimiento_modulos.service'<br/>
- Ejecutar 'sudo systemctl reenable mqtt_seguimiento_modulos.service'
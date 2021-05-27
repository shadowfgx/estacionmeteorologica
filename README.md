# Estacion Meteorologica

![estacion](https://user-images.githubusercontent.com/56632277/119800729-55b66a00-bedd-11eb-9327-5e24706d526d.png)

![image](https://user-images.githubusercontent.com/56632277/119800874-75e62900-bedd-11eb-8521-952e1b173953.png)

CÓDIGO Y LIBRERÍAS

El código fuente del proyecto se encuentra en la plataforma GitHub en este repositorio. Con lo que respecta a la programación se uso para la lectura de datos del sensor y para la conexiónde este a la plataforma ThingsBoard el IDE de Arduino dado que es el que proporciona mayor sencillez para la programación así como una variedad de librerías mayor.
A continuación podemos ver una tabla en la que se especifica el uso de las librerías que se han usado así como la versión de estas:

![image](https://user-images.githubusercontent.com/56632277/119802247-b003fa80-bede-11eb-8aaf-d1c7f26a6dae.png)


Para el correcto funcionamiento se ha implementado un sistema que realiza lecturas y acciones dependiendo de estas lecturas, envio de datos a la plataforma ThingsBoard y hacia el bot de Telegram creado con BotFather, para la implementacion del sistema se ha utilizado las librerias antes mencionadas.

Enlace al código fuente. https://github.com/shadowekko/estacionmeteorologica/blob/main/main/estacionmeteorologicav1.ino

PLATAFORMA IOT

Para poder visualizar los datos recopilados por los cinco sensores utilizados en este proyecto se ha utilizado la plataforma Thingsboard. Thingsboardies una plataforma IoT de código abierto que permite, entre otras funciones,la recopilación y visualización de los datos administrados por dispositivos. A continuación, se explicarán los procedimientos utilizados para la creación de los dispositivos y las gráficas para la visualización de los datos.PersistenciaEl primer paso para la utilización de la plataforma de Thingsboard es la creación de un dispositivo receptor de los datos. Para ello se utilizará la interfaz de usuario para agregar el dispositivomanualmente. 
A continuación, para poder conectar el dispositivo, se deben obtener las credenciales del dispositivo creado, para ello se accederá a los datos del dispositivo y se procederá a copiar el identificador del dispositivo llamado “token access”. 
Este identificador es único para cada dispositivo y se utilizará para, a través de comandos simples, enviar los datos desde los sensores al dispositivo.En este proyecto se ha procedido a la creación de un solo dispositivo denominado “datos del sensor” del tipo “sensores” mediante el cual, a través de diferentes variables, se procesarán los datosde los cinco sensores disponibles.
Dashboards o paneles de visualización de datosPara la visualización de los datos obtenidos por los sensores la plataforma Thingsboard dispone de paneles de visualizaciónde datos. Se procede a la creación de un panel llamado “estación” en el cual se crearán las gráficasde visualización de los datos de cada sensor.

![image](https://user-images.githubusercontent.com/56632277/119801423-fdcc3300-bedd-11eb-941b-701c397dfcf6.png)


HARDWARE

El sistema IOT se basa en una placa ESP32. A continuación se listan los componentes hardware usados.
  •Placa ESP32
  •Placa de pruebas
  •Grove air quality sensor v3
  •Grove light sensor v1.2
  •Grove Loudness sensor v0.9b
  •DHT 22
  •GUVAAnalog UV Light Sensor
También se requiere un cable USB para conectar la placa y múltiples cables para conectar los componentes entre sí.
Las conexiones en la placa de pruebas son las especificadas en la siguiente figura:

![image](https://user-images.githubusercontent.com/56632277/119801836-556a9e80-bede-11eb-9dfc-4591c92ee348.png)



# Five-Axix-Robotic-Arm

## Descripción del proyecto

El presente proyecto consiste en un brazo robótico de 5 ejes al cual se le pueden programar una serie de pasos que realiza hasta que se le de la instrucción de abortar.

El brazo cuenta con un botón que sirve para fallos de emergencias.

Además cuenta con un led que nos notifica el estado del brazo.

Los 5 ejes con los que cuenta el brazo son:

* La "Base", o "Cintura" capáz de girar 360°
* El "Hombro" es el responsable de "levantar o bajar" el brazo verticalmente
* El "codo" hace que el brazo "avance o retroceda".
* La Muǹeca hace que “gire” la Pinza.
* La "Garra" o "Pinza" funciona abriendo o cerrándose para "agarrar cosas".

## Información general de los autores

> **Universidad:** Instituto Tecnológico de León
>
> **Carrera:** Ingeniería en Sistemas Computacionales
>
> **Materia:** Sistemas Programables
>
> **Periodo:** Agosto-Diciembre 2018
>
> **Profesor:** Ing. Levy Rojas Carlos Rafael
> 
 ### Integrantes:
 * Campos Magallanes Ana Luisa - [AniluCampos](https://github.com/AniluCampos)
 * Dávalos Romero Daniel - [MrDanii](https://github.com/MrDanii)
 * Esparza Guevara Nestor Paul - [PaulEsparza](https://github.com/PaulEsparza)

## Pre-requisitos 
### Materiales
> * Protoboard
> * Arduino MEGA
> * Papel Cascaron
> * Alambre
> * Palillos de madera
> * 2 Taparroscas
> * 2 servomotores futaba S3003
> * 1 servomotor Tower Pro MG995
> * 1 servomotor Tower Pro SG90
> * 1 motor a pasos
> * Cables macho-macho
> * Cables macho-hembra
> * 1 pushbotton
> * 1 puente h L293d
> * 1 led
> * 3 resistencias de 1k ohms
 
 ### Librerías Arduino
 **<Servo.h>** para servomotores - disponible [aquí](https://www.arduinolibraries.info/libraries/servo)
 
 **<Stepper.h>** para motor a pasos - disponible [aquí](https://www.arduinolibraries.info/libraries/stepper)

 ### Librerías Java 
 **RXTX** - disponible [aquí](http://rxtx.qbang.org/wiki/index.php/Download)
 
 ## Procedimiento
 
 **1. Armar el circuido de la siguiente manera**

 ![](/Imagenes/Circuito1.jpg)
 ![](/Imagenes/Circuito2.jpg)
 
 **Armar el robót de la siguiente manera**

  ![](/Imagenes/Foto1.jpg)

 **2. Importar las librerias en los IDE correspondientes**
 
 **3. Descargar y ejecutar los códigos en las clases correspondientes**

 >Abrir el archivo [test_motor.ino](https://github.com/MrDanii/Five-Axix-Robotic-Arm/blob/master/test_motor/test_motor.ino) en el IDE de Arduino y ejecutarlo.
 
 >Compilar el proyecto java [BrazoRobotico](https://github.com/MrDanii/Five-Axix-Robotic-Arm/tree/master/BrazoRobotico) y ejecutar la clase PruebaBrazoRobotico.java ubicada en
    src/BrazoRobotico/

 >Se ejecutará la siguiente interfaz gráfica con la que podremos controlar el brazo

   ![](/Imagenes/Interfaz.png)

Con los sliders podemos configurar el ángulo en el que queremos la muñeca, el codo, el brazo y la pinza.

Con los botones 'Izquiera' y 'Derecha' controlamos el ángulo hacia el que apunta la base.

Con el botón 'Bloquear' se bloquea el brazo de manera que no se puedan ejecutar los pasos que seleccionemos.
Con el botón 'Desbloquear' lo desbloqueamos.
Con el botón 'Play' comenzamos a grabar los pasos que programemos a continuacion.
Con el botón 'Stop' dejamos de programar los movimientos.
Con el botón 'Reproducir' ejecutamos los pasos programados anteriormente.
Con el botón 'Abortar' detenemos la ejecución de los pasos programados.


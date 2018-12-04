/*
 * *******************************************************************************************
 * Este codigo permite controlar un brazo robotico a traves de una lectura del monitor serie *
 * de datos enviados desde una interfaz realizada en java netbeans.                          *
 * @autor Campos Magallanes Ana Luisa                                                        *
 * @autor Davalos Romero Daniel                                                              *
 * @autor Esparza Guevara Nestor Paul                                                        *
 * Libreria RXTXcomm                                                                         *
 * *******************************************************************************************
 */
 
/*
    Definimos las librerias que se van a utilizar, una para
    controlar el movimiento de los servomotores (Servo.h)
    y otra pa controlar el motor a pasos (Stepper.h)
    respectivamente
*/
#include <Servo.h>
#include <Stepper.h>

/*
   Definimos las variables de los servomotores junto con los
   pin's utilizados en la placa arduino
   ServoP - pinza
   ServoM - muñeca
   ServoC - codo
   ServoB - brazo
*/
Servo ServoP;
Servo ServoM;
Servo ServoC;
Servo ServoB;

/*
    Definmos una posicion inicial del brazo robotico
    p - pinza
    m - muñeca
    c - codo
    b - brazo
*/
int p = 2;
int m = 100;
int c = 140;
int b = 40;
int estado; // Variable de lectura de boton
int bloqueo = 0; // Variable para bloquear ejecucion de brazo dependiendo de su valor
String dato; // Variable para almacenar movimientos del brazo
boolean demo = false; // Variable que determina reproducir o no la secuencia grabada para el brazo
String arreglo[500]; // Arreglo que ayudara a guardar la secuencia grabada por el usuario
boolean grabar = false; // Variable que dependiendo de su valor nos permitira grabar una secuencia de movimientos o no
int cont = 0; // Variable que almacena las pocisiones grabadas en la secuencia

const int BOTON = 29; // Pin de boton
const int LEDVERDE = 31; // Pin led verde
const int LEDAMARILLO = 33; // Pin led amarillo
const int LEDROJO = 35; // Pin led rojo

#define STEPS 200 //Ponemos el número de pasos que necesita para dar una vuelta nuestro motor 200 en nuestro caso 
// Ponemos nombre la variable del motor a pasos, el número de pasos y los pins de control
Stepper stepper(STEPS, 4, 5, 6, 7); //Stepper nombre motor (número de pasos por vuelta, pins de control)

void setup() {
  /*
     Definicion de pines para control de servos
  */
  ServoP.attach(9);
  ServoM.attach(10);
  ServoC.attach(11);
  ServoB.attach(12);
  stepper.setSpeed(50); // Velocidad de giro del servomotor
  
  /*
     Definimos los tipos (entrada - salida) de cada componente utilizado
  */
  pinMode(BOTON, INPUT_PULLUP);
  pinMode(LEDVERDE, OUTPUT);
  pinMode(LEDAMARILLO, OUTPUT);
  pinMode(LEDROJO, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LEDVERDE, LOW);
  digitalWrite(LEDAMARILLO, HIGH);
  digitalWrite(LEDROJO, HIGH);
  digitalWrite(LED_BUILTIN, LOW);


  Serial.begin(9600); // Inicializamos la conexion del puerto serial
  Serial.setTimeout(10); // Defnimos el tiempo de espera serial para leer datos
}

/*
   A traves de diferentes estructuras condicionales mandamos a llamar
   a los diferentes metodos para que cumplan con sus respectivas funcionalidades
   especificadas en cada metodo
*/
void loop() {
  estado = digitalRead(BOTON); // Almacenar variable boton en estado para bloquear o desbloquear
  abortaBoton(); // Metodo que permite abortar la funcionalidad del brazo desde un boton fisico
  
  /*
     Estructura condicional que evalua la variable demo,
     si esta es true ejecuta la rutina grabada por el usuario en caso contrario no se ejecuta
     y el brazo se mantendra en un manejo manual
  */
  if (demo) {
    rutina(); // Metodo que ejecuta la rutina grabada del brazo
  }
  
  /*
     Estructura condicional que evalua la variable grabar,
     si esta es true comienza a grabarse una rutina para el movimiento del brazo
     en caso contrario no se ejecuta
  */
  if (grabar) {
    if (Serial.available()) { //Comprobamos si en el buffer hay datos
      if (cont < 500) { // Si el contador es menor al espacio de datos disponibles en el arreglo
        arreglo[cont++] = dato; // Almacenamos los datos ingresados para grabar la secuencia del brazo
      }
    }
  }

  if (Serial.available()) { //Comprobamos si en el buffer hay datos
    dato = Serial.readStringUntil('\n'); // Variable que almacena los datos enviados desde la interfaz java despues de leer un salto de linea
    abortaBotonInterfaz(); // Metodo que permite abortar la funcionalidad del brazo desde un boton en la interfaz utilizada
    lecturaMovimientoBrazo(); // Metodo que permite el movimiento del brazo dependiendo del motor que se requiera utilizar
  }
  lecturaLeds(); // Metodo que permite dar informacion fisica al usuario del estado en el que se encentra el brazo
  rangoMovimientoBrazo(); // Metodo que establece los limites de movimiento del brazo en cada motor
  
  /*
     Valores LOW para que el motor a pasos quede con giro libre y evitar sobrecalentamiento en ellos
  */
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
}

/*
  Metodo para la funcionalidad del boton fisico para
  detener la grabacion en caso de que se este ejecutando.
  Este hace lectura al oprimir el boton para incrementar
  el valor de la variable bloqueo y asi desactivar el demo que este en ejecucion
*/
void abortaBoton() {
  if (estado == LOW) { //Boton apretado
    while (estado == LOW) {
      estado = digitalRead(BOTON);
    }
    delay(10);
    bloqueo++; // Incremento de variable que nos sirve dependiendo su valor encender led con su color correspondiente
    demo = false; // Cambio de dato de variable que se utiliza para ejecutar o parar la rutina de demostracion del brazo
  }
}

/*
  Metodo para abortar la agrabacion que este ejecutando el brazo robotico
  en caso de que se oprima el boton desde la interfaz(El boton al ser oprimido envia un
  caracter para ser leido desde la interfaz serial de arduino).
  Se utiliza una estructura condicional para terminar la secuencia
*/
void abortaBotonInterfaz() {
  if (dato.substring(0, 1) == "S")
  {
    bloqueo++; // Incremento de variable que nos sirve dependiendo su valor encender led con su color correspondiente
    demo = false; // Cambio de dato de variable que se utiliza para ejecutar o parar la rutina de demostracion del brazo
  }
}

/*
     Metodo que permite realizar movimiento al brazo.
     Su estructura condicional principal evalua dos variables(bloqueo - demo)
     Si el bloqueo esta desactivado(igual a cero) y demo esta desactivado(valor false)
     Prende led verde, apaga los demas y permite hacer uso del brazo
*/
void lecturaMovimientoBrazo() {
  if (bloqueo == 0 && demo == false) {
    if (grabar == false) { // Si el brazo no esta en modo de grabacion, el led verde se mantiene encendido
      digitalWrite(LEDROJO, HIGH);
      digitalWrite(LEDVERDE, LOW);
      digitalWrite(LEDAMARILLO, HIGH);
    }
    
    /*
       Condicionales para leer movimiento de servomotores
       P - pinza
       M - muñeca
       C - Codo
       B - brazo
       Esto para cada dato que arduino lea por serial sea identificado y
       mueva el servo correspondiente a dicha instruccion con su grado correspondiente
    */
    if (dato.substring(0, 1) == "P")
    {
      p = dato.substring(1).toInt();
    }
    else if (dato.substring(0, 1) == "M")
    {
      m = dato.substring(1).toInt();
    }
    else if (dato.substring(0, 1) == "C")
    {
      c = dato.substring(1).toInt();
    }
    else if (dato.substring(0, 1) == "B")
    {
      b = dato.substring(1).toInt();
    }
    
    /*
       Condicionales para el movimiento de motor a pasos(izquierda - derecha)
       su movimiento sera de 20 grados en este caso cada que el boton de la interfaz
       sea oprimido
    */
    else if (dato.substring(0, 1) == "I")
    {
      stepper.step(-20);
    }
    else if (dato.substring(0, 1) == "D")
    {
      stepper.step(20);
    }
    
    // Condicional para activar la rutina de grabacion en caso de que el valor recibido sea R
    else if (dato.substring(0, 1) == "R")
    {
      demo = true; // Bandera que permite que la grabacion sea ejecutada
    }
    
    // Condicional para activar el funcionamiento de grabacion en caso de que el valor recibido sea G
    else if (dato.substring(0, 1) == "G")
    {
      cont = 0; // Variable que nos limpia los datos si existe una rutina grabada anteriormente
      grabar = true; // Bandera que permite almacenar los movimientos del brazo mientras sea verdadera
      // Informacion fisica al usuario con el led en amarillo (estado de grabacion)
      digitalWrite(LEDROJO, LOW);
      digitalWrite(LEDVERDE, LOW);
      digitalWrite(LEDAMARILLO, HIGH);
    }
    
    // Condicional para desactivar el modo de grabado en caso de recibir el valor H
    else if (dato.substring(0, 1) == "H")
    {
      grabar = false; // Bandera que evita que los movimientos del brazo se graben cuando su valor es falso
      // Informacion fisica al usuario con el led en verde (estado manual)
      digitalWrite(LEDROJO, HIGH);
      digitalWrite(LEDVERDE, LOW);
      digitalWrite(LEDAMARILLO, HIGH);

    }
  }
}

/*
  Metodo que lee el estado de cada led para otorgar informacion fisica para el brazo
  Bloqueo = 1, Led rojo, el brazo bloqueado
  Bloqueo = 2, asigna el valor 0 a la variable bloqueo(desactivar) y permite hacer uso del brazo
*/
void lecturaLeds() {
  if (bloqueo == 1) {
    // Informacion fisica al usuario con el led en rojo (estado de bloqueo)
    digitalWrite(LEDROJO, LOW);
    digitalWrite(LEDVERDE, HIGH);
    digitalWrite(LEDAMARILLO, HIGH);
  }
  else if (bloqueo == 2) {
    bloqueo = 0;
    // Informacion fisica al usuario con el led en verde (estado manual)
    digitalWrite(LEDROJO, HIGH);
    digitalWrite(LEDAMARILLO, HIGH);
    digitalWrite(LEDVERDE, LOW);
  }
}

/*
   Metodo para leer los valores admitidos para el movimiento del brazo
   y ejecutar su movimiento de acuerdo a su valor actual
*/
void rangoMovimientoBrazo() {
  if (p >= 2 && p <= 30) {
    ServoP.write(p);
  }
  if (m >= 25 && m <= 150) {
    ServoM.write(m);
  }
  if (c >= 85 && c <= 145) {
    ServoC.write(c);
  }
  if (b >= 40 && b <= 100) {
    ServoB.write(b);
  }
}

/*
   Metodo de secuencia de grabacion predefinida para que sea llevada a cabo por el brazo
   escribiendo sus valores en los servomotores para ser ejecutados.
   (Se mantiene el led amarillo prendido para informacion fisica de ejecucion de la grabacion)
*/
void rutina() {
  digitalWrite(LEDROJO, HIGH);
  digitalWrite(LEDVERDE, HIGH);
  digitalWrite(LEDAMARILLO, LOW);
  grabar = false; // Bandera para evitar que los movimientos sean grabados
  
  /*
     Condicionales para leer movimiento de servomotores en el arreglo de movimientos
     P - pinza
     M - muñeca
     C - Codo
     B - brazo
     Esto para cada dato que arduino lea sea identificado y mueva el servo correspondiente
     a dicha instruccion con su grado correspondiente
  */
  for (int i = 1; i < cont; i++) {
    delay(100);
    if (arreglo[i].substring(0, 1) == "P")
    {
      p = arreglo[i].substring(1).toInt();
    }
    else if (arreglo[i].substring(0, 1) == "M")
    {
      m = arreglo[i].substring(1).toInt();
    }
    else if (arreglo[i].substring(0, 1) == "C")
    {
      c = arreglo[i].substring(1).toInt();
    }
    else if (arreglo[i].substring(0, 1) == "B")
    {
      b = arreglo[i].substring(1).toInt();
    }
    
    /*
       Condicionales para el movimiento de motor a pasos(izquierda - derecha)
       su movimiento sera de 20 grados en este caso cada que la grabacion de rutina
       ejecutada lo requiera
    */
    else if (arreglo[i].substring(0, 1) == "I")
    {
      stepper.step(-20);
      delay(50);
    }
    else if (arreglo[i].substring(0, 1) == "D")
    {
      stepper.step(20);
      delay(50);
    }
    rangoMovimientoBrazo(); // Metodo que establece los limites de movimiento del brazo en cada motor
  }
}

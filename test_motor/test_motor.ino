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
boolean demo = false; // Variable que determina si el modo demo para el brazo esta activado

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
  /*
     Valores por default de inicio para cada led
  */
  digitalWrite(LEDVERDE, LOW);
  digitalWrite(LEDAMARILLO, HIGH);
  digitalWrite(LEDROJO, HIGH);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600); // Inicializamos la conexion del puerto serial
  Serial.setTimeout(10); // Tiempo de espera serial para leer datos

}

/*
   A traves de diferentes estructuras condicionales mandamos a llamar
   a los diferentes metodos para que cumplan con sus respectivas funcionalidades
   especificadas en cada metodo
*/
void loop() {
  estado = digitalRead(BOTON); // Almacenar variable boton en estado para bloquear o desbloquear
  abortaBoton();
  /*
     Estructura condicional que evalua la variable demo,
     si esta es true ejecuta la rutina en caso contrario no se ejecuta
  */
  if (demo) {
    rutina();
  }

  if (Serial.available()) { //Comprobamos si en el buffer hay datos
    dato = Serial.readStringUntil('\n'); // Variable que almacena los datos enviados desde la interfaz java despues de leer un salto de linea
    abortaBotonInterfaz();
    lecturaMovimientoBrazo();
  }
  lecturaLeds();
  rangoMovimientoBrazo();
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
  detener la demostracion en caso de que se este ejecutando.
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
  Metodo para abortar el demo que este ejecutando el brazo robotico
  en caso de que se oprima el boton desde la interfaz(El boton al ser oprimido envia un
  caracter para ser leido desde la interfaz serial de arduino).
  Se utiliza una estructura condicional para terminar el demo
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
    digitalWrite(LEDROJO, HIGH);
    digitalWrite(LEDVERDE, LOW);
    digitalWrite(LEDAMARILLO, HIGH);
    /*
       Condicionales para leer movimiento de servomotores
       P - pinza
       M - muñeca
       C - Codo
       B - brazo
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
    // Condicional para activar la rutina demo en caso de que el valor recibido sea R
    else if (dato.substring(0, 1) == "R")
    {
      demo = true;
    }
  }
}

/*
  Metodo que lee el estado de cada led para otorgar informacion fisica para el brazo
  Bloqueo = 1, Led rojo, el brazo bloqueado
  Bloqueo = 2, asigna el valor 0 a la variable bloqueo(desactivar) y permite hacer uso del brazo
  **El valor leido de los led's es el opuesto ya que su RGB tiene el positivo en comun
*/
void lecturaLeds() {
  if (bloqueo == 1) {
    digitalWrite(LEDROJO, LOW);
    digitalWrite(LEDVERDE, HIGH);
    digitalWrite(LEDAMARILLO, HIGH);
  }
  else if (bloqueo == 2) {
    bloqueo = 0;
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
   Metodo de rutina predefinida para que sea llevada a cabo por el brazo
   escribiendo sus valores en los servomotores para ser ejecutados.
   (Se mantiene el led amarillo prendido para informacion fisica de ejecucion del demo)
*/
void rutina() {
  digitalWrite(LEDROJO, HIGH);
  digitalWrite(LEDVERDE, HIGH);
  digitalWrite(LEDAMARILLO, LOW);
  /*
     Descripcion de ciclos para la demostracion:
     Baja la pinza
     Cierra piinza
     Levanta pinza
     abre pinza
  */
  for (int l = 150; l >= 25; l--) {
    ServoM.write(l);
    delay(10);
  }
  for (int i = 2; i <= 30; i++) {
    ServoP.write(i);
    delay(25);
  }
  for (int k = 25; k <= 150; k++) {
    ServoM.write(k);
    delay(10);
  }
  for (int j = 30; j >= 2; j--) {
    ServoP.write(j);
    delay(25);
  }
}

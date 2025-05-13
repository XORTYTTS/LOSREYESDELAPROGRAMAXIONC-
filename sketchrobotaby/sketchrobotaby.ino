#include <SoftwareSerial.h>

// Pines del L298N
const int ENA = 9;
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 6;
const int ENB = 5;

// Sensor ultrasónico
const int TRIG = 7;
const int ECHO = 8;
const int DISTANCIA_UMBRAL = 10;

// Módulo Bluetooth
SoftwareSerial BT(10, 11); // RX, TX
const int BT_STATE = 12;   // Pin conectado al pin STATE del HC-05

bool modoAutomatico = true;

void setup() {
  // Motores
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Sensor ultrasónico
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Bluetooth
  pinMode(BT_STATE, INPUT);
  BT.begin(9600);

  // Velocidad inicial
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
}

void loop() {
  // Desactiva el modo automático si se detecta conexión Bluetooth
  if (digitalRead(BT_STATE) == HIGH) {
    modoAutomatico = false;
  }

  // Leer comando Bluetooth si hay
  if (BT.available()) {
    char comando = BT.read();
    manejarComando(comando);
  }

  // Modo automático si no hay conexión y está activado
  if (modoAutomatico) {
    long distancia = medirDistanciaEstable();

    if (distancia > 0 && distancia < DISTANCIA_UMBRAL) {
      retroceder();
      delay(700);
      girarDerecha();
      delay(700);
    } else {
      avanzar();
    }

    delay(50);
  }
}

// Procesar comandos del celular
void manejarComando(char c) {
  c = toupper(c);

  switch (c) {
    case 'F':
      avanzar();
      break;
    case 'B':
      retroceder();
      break;
    case 'L':
      girarIzquierda();
      break;
    case 'R':
      girarDerecha();
      break;
    case 'S':
      detener();
      break;
    case 'A':
      modoAutomatico = true;  // Reactivar sensor si se desea
      break;
  }
}

// Movimiento
void avanzar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void retroceder() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void girarDerecha() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void girarIzquierda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Sensor ultrasónico
long medirDistanciaEstable() {
  long suma = 0;
  for (int i = 0; i < 5; i++) {
    suma += medirDistancia();
    delay(10);
  }
  return suma / 5;
}

long medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracion = pulseIn(ECHO, HIGH, 30000);
  if (duracion == 0) return 999;

  long distancia = duracion * 0.034 / 2;
  return distancia;
}

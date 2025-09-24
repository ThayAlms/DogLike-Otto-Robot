#include <RoboCore_Vespa.h>
#include <math.h>

VespaServo servos[4]; // Move vários servos simultaneamente de forma suave

// ----- DIMENSÕES -----
const float L = 73.0;  // mm - largura entre eixos
const float C = 47.0;  // mm - comprimento entre eixos
const float P = 27.0;  // mm - comprimento da perna
const float H = 6.0;   // mm - altura desejada do corpo

// ----- CONFIG SERVOS -----
const int S_PIN[4] = {26, 25, 33, 32}; // frente esq, frente dir, trás esq, trás dir
float curAngle[4];

// ----- MOVIMENTO -----
const float STEP_X = 15.0;  // passo para frente (mm)
const int STEP_DELAY = 7;   // ms entre micro-passos
float walkSpeed = 1.7;      // velocidade global (quanto maior, mais rápido)

// ----- PID SIMPLES -----
float Kp = 0.6;  // proporcional
float Kd = 0.05; // derivativo
float lastError[4] = {0,0,0,0};

// --- Função para converter coordenada em ângulo ---
int calcServoAngle(float xNeutral, float xTarget, bool invert) {
  float delta = xTarget - xNeutral;
  float percent = delta / STEP_X;
  int angle = 90 + percent * 40;  // amplitude de 40°
  if (invert) angle = 180 - angle;
  return constrain(angle, 0, 180);
}

// Move suavemente todos os servos para ângulos desejados com PID
void moveLegsSmooth(int target[4]) {
  bool done = false;
  while (!done) {
    done = true;
    for (int i = 0; i < 4; i++) {
      float error = target[i] - curAngle[i];
      float derivative = (error - lastError[i]);
      float step = (Kp * error) + (Kd * derivative);
      step = constrain(step, -walkSpeed, walkSpeed); // limita passo
      curAngle[i] += step;
      curAngle[i] = constrain(curAngle[i], 0, 180);
      servos[i].write((int)curAngle[i]);
      lastError[i] = error;

      if (fabs(error) > 1) done = false;
    }
    delay(STEP_DELAY);
  }
}

// Posição neutra
void goNeutral() {
  int target[4] = {90, 90, 90, 90};
  moveLegsSmooth(target);
}

// Trote: frente junto → trás junto
void trotForward() {
  float xNeutral[4] = {+C/2, +C/2, -C/2, -C/2};

  // Passo frente (S1 + S2 para frente)
  int target[4] = {
    calcServoAngle(xNeutral[0], xNeutral[0] - STEP_X, false),
    calcServoAngle(xNeutral[1], xNeutral[1] - STEP_X, true),
    90, 90
  };
  moveLegsSmooth(target);

  goNeutral(); // volta ao neutro antes do próximo par

  // Passo trás (S3 + S4 para frente)
  target[0] = 90;
  target[1] = 90;
  target[2] = calcServoAngle(xNeutral[2], xNeutral[2] - STEP_X, false);
  target[3] = calcServoAngle(xNeutral[3], xNeutral[3] - STEP_X, true);
  moveLegsSmooth(target);

  goNeutral();
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    servos[i].attach(S_PIN[i]);
    curAngle[i] = 90;
    servos[i].write(90);
  }
  delay(500);
  goNeutral();
}

void loop() {
  trotForward(); // executa o trote continuamente
  delay(100);    // pausa curta antes de repetir
}

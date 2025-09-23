#include <RoboCore_Vespa.h>
#include <math.h>

VespaServo servos[4]; // Move vários servos simultaneamente de forma suave

// ----- DIMENSÕES -----
const float L = 73.0;  // mm - largura entre eixos
const float C = 47.0;  // mm - comprimento entre eixos
const float P = 27.0;  // mm - comprimento da perna
const float H = 6.0;   // mm - altura desejada do corpo

// ----- REFERÊNCIA -----
const int NEUTRO = 90;

const int S1_FRENTE = 0;     
const int S1_BACK   = 180;

const int S2_FRENTE = 0;   
const int S2_BACK   = 180;

const int S3_FRENTE = 180;     
const int S3_BACK   = 0;

const int S4_FRENTE = 180;   
const int S4_BACK   = 0;

// ----- CONFIG SERVOS -----
const int S_PIN[4] = {26, 25, 33, 32}; // frente esq, frente dir, trás esq, trás dir
float curAngle[4];

// ----- MOVIMENTO -----
const float STEP_X = 15.0;  // passo para frente (mm)
const int STEP_DELAY = 7;   // ms entre micro-passos
float walkSpeed = 0.8;      // velocidade global (quanto maior, mais rápido)

// ----- PID SIMPLES -----
float Kp = 0.6;  // proporcional: ajuste principal
float Kd = 0.05; // derivativo: suaviza frenagem
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
      step = constrain(step, -walkSpeed, walkSpeed); // limita passo para suavidade
      curAngle[i] += step;
      curAngle[i] = constrain(curAngle[i], 0, 180);
      servos[i].write((int)curAngle[i]);
      lastError[i] = error;

      if (fabs(error) > 1) done = false; // ainda não chegou no alvo
    }
    delay(STEP_DELAY);
  }
}

// Posição neutra (corpo centralizado)
void goNeutral() {
  int target[4] = {90, 90, 90, 90};
  moveLegsSmooth(target);
}

// Caminhada baseada em coordenadas
void walkForward() {
  float xNeutral[4] = {+C/2, +C/2, -C/2, -C/2};

  // Perna 1 frente esq (AGORA AVANÇA PARA FRENTE)
  int target[4] = {
    calcServoAngle(xNeutral[0], xNeutral[0] - STEP_X, false), // TROQUEI + por -
    90, 90, 90
  };
  moveLegsSmooth(target);

  // Perna 3 trás dir
  target[0] = 90;
  target[3] = calcServoAngle(xNeutral[3], xNeutral[3] - STEP_X, true); // TROQUEI + por -
  moveLegsSmooth(target);

  // Frente dir recua (AGORA RECUSA PARA TRÁS)
  target[3] = 90;
  target[1] = calcServoAngle(xNeutral[1], xNeutral[1] + STEP_X, true); // TROQUEI - por +
  moveLegsSmooth(target);

  // Trás esq recua
  target[1] = 90;
  target[2] = calcServoAngle(xNeutral[2], xNeutral[2] + STEP_X, false); // TROQUEI - por +
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
  walkForward();
}

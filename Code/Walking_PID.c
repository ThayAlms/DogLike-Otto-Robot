#include <RoboCore_Vespa.h>
#include <math.h>

VespaServo servos[4];// Move vários servos simultaneamente de forma suave.

// ----- DIMENSÕES -----
const float L = 73.0;  // mm - largura entre eixos
const float C = 47.0;  // mm - comprimento entre eixos
const float P = 27.0;  // mm - comprimento da perna
const float H = 6.0;  // mm - altura desejada do corpo

//REFERÊNCIA

const int NEUTRO = 90;

const int S1_FRENTE = 0;     
const int S1_BACK  = 180;

const int S2_FRENTE = 0;   
const int S2_BACK  = 180;

const int S3_FRENTE = 180;     
const int S3_BACK  = 0;

const int S4_FRENTE = 180;   
const int S4_BACK  = 0;

// ----- CONFIG SERVOS -----
const int S_PIN[4] = {26, 25, 33, 32}; // frente esq, frente dir, trás esq, trás dir
float curAngle[4];

// ----- MOVIMENTO -----
const float STEP_X = 20.0;  // passo para frente (mm)
const float LIFT = 10.0;     // elevação simulada (mm)
const int STEP_DELAY = 10;  // ms entre micro-passos

// --- Função para converter coordenada X em ângulo servo ---
// Para robô de 1 DOF por perna, simplificamos: usamos relação linear
// (ângulo = mapeamento entre posição X e ângulo)
int calcServoAngle(float xNeutral, float xTarget, bool invert) {
  float delta = xTarget - xNeutral;
  float percent = delta / STEP_X; // normaliza
  int angle = 90 + percent * 40;  // 40° de amplitude para frente/trás
  if (invert) angle = 180 - angle;
  angle = constrain(angle, 0, 180);
  return angle;
}

// Move suavemente todos os servos para ângulos desejados
void moveLegsSmooth(int target[4]) {
  bool done = false;
  while (!done) {
    done = true;
    for (int i = 0; i < 4; i++) {
      if (abs(curAngle[i] - target[i]) > 1) {
        done = false;
        if (curAngle[i] < target[i]) curAngle[i] += 1;
        else curAngle[i] -= 1;
        servos[i].write((int)curAngle[i]);
      }
    }
    delay(STEP_DELAY);
  }
}

// Posição neutra (corpo centralizado)
void goNeutral() {
  int target[4] = {90, 90, 90, 90}; // neutro
  moveLegsSmooth(target);
}

// Caminhada baseada em coordenadas
void walkForward() {
  // Ponto neutro para cada perna
  float xNeutral[4] = {+C/2, +C/2, -C/2, -C/2};

  // --- SEQUÊNCIA DE PASSO ---
  // Perna 1 (frente esq) avança
  int target[4] = {
    calcServoAngle(xNeutral[0], xNeutral[0] + STEP_X, false),
    90, 90, 90
  };
  moveLegsSmooth(target);

  // Perna 3 (trás dir) avança
  target[0] = 90;
  target[3] = calcServoAngle(xNeutral[3], xNeutral[3] + STEP_X, true);
  moveLegsSmooth(target);

  // Frente dir recua
  target[3] = 90;
  target[1] = calcServoAngle(xNeutral[1], xNeutral[1] - STEP_X, true);
  moveLegsSmooth(target);

  // Trás esq recua
  target[1] = 90;
  target[2] = calcServoAngle(xNeutral[2], xNeutral[2] - STEP_X, false);
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

#include <RoboCore_Vespa.h>
#include <math.h>

VespaServo servos[4];

// ----- DIMENSÕES -----
const float L = 73.0;
const float C = 47.0;
const float P = 27.0;
const float H = 6.0;

// ----- CONFIG SERVOS -----
const int S_PIN[4] = {26, 25, 33, 32}; 
float curAngle[4];

// ----- MOVIMENTO -----
const float STEP_X = 15.0;
const int STEP_DELAY = 7;
float walkSpeed = 2.7;

// ----- PID -----
float Kp = 0.6;
float Kd = 0.05;
float lastError[4] = {0,0,0,0};

// --- Converte coordenada para ângulo ---
int calcServoAngle(float xNeutral, float xTarget, bool invert) {
  float delta = xTarget - xNeutral;
  float percent = delta / STEP_X;
  int angle = 90 + percent * 40;
  if (invert) angle = 180 - angle;
  return constrain(angle, 0, 180);
}

// --- Move todos os servos suavemente ---
void moveLegsSmooth(int target[4]) {
  bool done = false;
  while (!done) {
    done = true;
    for (int i = 0; i < 4; i++) {
      float error = target[i] - curAngle[i];
      float derivative = (error - lastError[i]);
      float step = (Kp * error) + (Kd * derivative);
      step = constrain(step, -walkSpeed, walkSpeed);
      curAngle[i] += step;
      curAngle[i] = constrain(curAngle[i], 0, 180);
      servos[i].write((int)curAngle[i]);
      lastError[i] = error;

      if (fabs(error) > 1) done = false;
    }
    delay(STEP_DELAY);
  }
}

// --- Posição neutra ---
void goNeutral() {
  int target[4] = {90, 90, 90, 90};
  moveLegsSmooth(target);
}

// --- Caminhada em diagonais (S1+S4 e depois S2+S3) ---
void walkForward() {
  float xNeutral[4] = {+C/2, +C/2, -C/2, -C/2};

  // --- Etapa 1: S1 e S4 avançam juntos ---
  int target[4] = {
    calcServoAngle(xNeutral[0], xNeutral[0] - STEP_X, false), // S1 frente
    90,
    90,
    calcServoAngle(xNeutral[3], xNeutral[3] - STEP_X, true)   // S4 frente
  };
  moveLegsSmooth(target);

  // --- Volta para neutro ---
  goNeutral();

  // --- Etapa 2: S2 e S3 avançam juntos ---
  int target2[4] = {
    90,
    calcServoAngle(xNeutral[1], xNeutral[1] - STEP_X, true),  // S2 frente
    calcServoAngle(xNeutral[2], xNeutral[2] - STEP_X, false), // S3 frente
    90
  };
  moveLegsSmooth(target2);

  // --- Volta para neutro ---
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

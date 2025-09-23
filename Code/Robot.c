#include <RoboCore_Vespa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>
#include <math.h>

// Ligações do display
#define TFT_CS   5
#define TFT_DC   17
#define TFT_RST  16
#define TFT_SCLK 22
#define TFT_MOSI 21

VespaServo servos[4]; // Move vários servos simultaneamente de forma suave

// Construtor
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

int COLOR_WHITE = GC9A01A_BLACK;
int COLOR_BLACK = GC9A01A_YELLOW;

// Demo mode
int demo_mode = 1;
const int max_animation_index = 8;
int current_animation_index = 0;

// Tela
const int SCREEN_WIDTH = 240;
const int SCREEN_HEIGHT = 240;

// Referência dos olhos
int ref_eye_height = 60;
int ref_eye_width = 60;
int ref_space_between_eye = 20;
int ref_corner_radius = 15;

// Estado atual dos olhos
int left_eye_x, left_eye_y, right_eye_x, right_eye_y;
int left_eye_width, left_eye_height, right_eye_width, right_eye_height;
int corner_radius;

// ----- DIMENSÕES -----
const float L = 73.0;  // mm - largura entre eixos
const float C = 47.0;  // mm - comprimento entre eixos
const float P = 27.0;  // mm - comprimento da perna
const float H = 6.0;   // mm - altura desejada do corpo

// ----- REFERÊNCIA -----
const int NEUTRO = 90;

const int S1_FRENTE = 0;     
const int S1_BACK   = 180;

const int S2_FRENTE = 180;   
const int S2_BACK   = 0;

const int S3_FRENTE = 0;     
const int S3_BACK   = 180;

const int S4_FRENTE = 180;   
const int S4_BACK   = 0;

// Posições predefinidas
const int SIT_ANGLES[4] = {NEUTRO, NEUTRO, S3_FRENTE, S4_FRENTE};   // sentado
const int LAY_ANGLES[4] = {S1_BACK, S2_BACK, S3_FRENTE,S4_FRENTE};   // deitado
const int NEUTRO_ANGLES[4] = {90, 90, 90, 90};  // neutro

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

// Função para sentar (usando SIT_ANGLES)
void senta() {
  int target[4];
  for (int i = 0; i < 4; i++) {
    target[i] = SIT_ANGLES[i];
  }
  moveLegsSmooth(target);
}

// Função para deitar (usando LAY_ANGLES)
void deitar() {
  int target[4];
  for (int i = 0; i < 4; i++) {
    target[i] = LAY_ANGLES[i];
  }
  moveLegsSmooth(target);
}

// Função para dar patinha (levanta perna dianteira direita - S2, índice 1)
void darPatinha() {
  // Levanta perna
  int target[4] = {90, S2_FRENTE, 90, 90};  // S2 (frente dir) para 60°
  moveLegsSmooth(target);
  delay(500);  // Mantém levantada por 0.5s

  // Baixa perna (volta para posição sentada ou neutra; aqui usamos SIT_ANGLES[1] = 120)
  target[1] = SIT_ANGLES[1];
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

// ----------------------------------------------------------------
// Funções de Display
// ----------------------------------------------------------------
void display_clearDisplay() { tft.fillScreen(COLOR_BLACK); }

void display_fillRoundRect(int x,int y,int w,int h,int r,int color) {
  tft.fillRoundRect(x,y,w,h,r,color);
}

void display_fillTriangle(int x0,int y0,int x1,int y1,int x2,int y2,int color) {
  tft.fillTriangle(x0,y0,x1,y1,x2,y2,color);
}

// ----------------------------------------------------------------
// Olhos
// ----------------------------------------------------------------
void reset_eyes() {
  left_eye_width = right_eye_width = ref_eye_width;
  left_eye_height = right_eye_height = ref_eye_height;
  corner_radius = ref_corner_radius;

  left_eye_x = SCREEN_WIDTH/2 - ref_eye_width/2 - ref_space_between_eye/2;
  right_eye_x = SCREEN_WIDTH/2 + ref_eye_width/2 + ref_space_between_eye/2;
  left_eye_y = right_eye_y = SCREEN_HEIGHT/2;
  tft.fillScreen(GC9A01A_YELLOW);
  //display_clearDisplay();
}

void draw_eyes() {
  display_clearDisplay();
  display_fillRoundRect(left_eye_x - left_eye_width/2, left_eye_y - left_eye_height/2,
                        left_eye_width, left_eye_height, corner_radius, COLOR_WHITE);
  display_fillRoundRect(right_eye_x - right_eye_width/2, right_eye_y - right_eye_height/2,
                        right_eye_width, right_eye_height, corner_radius, COLOR_WHITE);
}

// ----------------------------------------------------------------
// Animações
// ----------------------------------------------------------------
void blink() {
  reset_eyes();
  for(int i=0;i<2;i++){
    left_eye_height = right_eye_height = 5;
    draw_eyes();
    reset_eyes();
    draw_eyes();
  }
}

void happy_eye() {
  reset_eyes();
  int offset = ref_eye_height/2;
  for(int i=0;i<5;i++){
    display_fillTriangle(left_eye_x-left_eye_width/2, left_eye_y+offset,
                         left_eye_x+left_eye_width/2, left_eye_y+5+offset,
                         left_eye_x-left_eye_width/2, left_eye_y+left_eye_height+offset,
                         COLOR_BLACK);
    display_fillTriangle(right_eye_x+right_eye_width/2, right_eye_y+offset,
                         right_eye_x-left_eye_width/2, right_eye_y+5+offset,
                         right_eye_x+right_eye_width/2, right_eye_y+right_eye_height+offset,
                         COLOR_BLACK);
    offset -= 4;
    draw_eyes();
    delay(50);
  }
  reset_eyes();
  draw_eyes();
}

void sleep() {
  reset_eyes();
  left_eye_height = right_eye_height = 2;
  corner_radius = 0;
  draw_eyes();
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(0);
  
  // Liga o display e mostra olhos parados
  reset_eyes();
  draw_eyes();
  delay(1000);  // Pausa para mostrar olhos parados
  
  // Inicializa servos
  for (int i = 0; i < 4; i++) {
    servos[i].attach(S_PIN[i]);
    curAngle[i] = 90;
    servos[i].write(90);
  }
  delay(500);
  goNeutral();
  
  // Andar para frente por alguns segundos (aprox. 20 passos, ~5-10s dependendo da velocidade)
  for (int step = 0; step < 5; step++) {
    walkForward();
  }
  
  // Sentar
  senta();
  delay(500);
  
  
  darPatinha();
  delay(500);
  
  // Deitar
  deitar();
  
  // Opcional: happy_eye() no final, se quiser
  // happy_eye();
}

void loop() {
  // Loop vazio; toda a sequência roda no setup(). Se quiser repetir, mova para loop().
  delay(1000);
  sleep();
}

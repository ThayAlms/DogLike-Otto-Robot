#include <RoboCore_Vespa.h>
#include <SPI.h>
#include <math.h>
#include <stdlib.h> // para usar random()
#include <LovyanGFX.hpp>

lgfx::LGFX_Device tft;

// ======================================================
// CONFIGURAÇÕES DO ROBÔ
// ======================================================
// Cores principais
#define COLOR_BACKGROUND TFT_BLUE  // fundo
#define COLOR_EYE        TFT_WHITE // olhos
#define COLOR_PUPIL      TFT_BLACK // pupilas ou detalhes

const int S_PIN[4] = {26, 25, 33, 32}; // S1, S2, S3, S4

// ----- ANGULOS -----
const int NEUTRO_ANGLES[4] = {90, 90, 90, 90};
const int SIT_ANGLES[4]    = {90, 90, 0, 180};
const int LAY_ANGLES[4]    = {180, 0, 0, 180};

// ----- MOVIMENTO -----
const float STEP_X = 15.0;
const int STEP_DELAY = 7;
float walkSpeed = 2.0;

// ----- PID -----
float Kp = 0.6;
float Kd = 0.05;

// ----- DISPLAY -----
const int SCREEN_WIDTH = 240;
const int SCREEN_HEIGHT = 240;

int ref_eye_height = 60;
int ref_eye_width = 60;
int ref_space_between_eye = 20;
int ref_corner_radius = 15;

int left_eye_x, left_eye_y, right_eye_x, right_eye_y;
int left_eye_width, left_eye_height, right_eye_width, right_eye_height;
int corner_radius;

// ======================================================
// FUNÇÕES AUXILIARES DE DISPLAY
// ======================================================
void display_clearDisplay() { tft.fillScreen(COLOR_BACKGROUND); }

void display_fillRoundRect(int x,int y,int w,int h,int r,uint16_t color) {
  tft.fillRoundRect(x, y, w, h, r, color);
}

void display_fillTriangle(int x0,int y0,int x1,int y1,int x2,int y2,uint16_t color) {
  tft.fillTriangle(x0,y0,x1,y1,x2,y2,color);
}

// ======================================================
// OBJETOS GLOBAIS
// ======================================================
VespaServo servos[4];
float curAngle[4] = {90, 90, 90, 90};
float lastError[4] = {0, 0, 0, 0};

// ======================================================
// FUNÇÕES DE MOVIMENTO
// ======================================================
void moveLegsSmooth(const int target[4]) {
  bool done = false;
  while (!done) {
    done = true;
    for (int i = 0; i < 4; i++) {
      float error = target[i] - curAngle[i];
      float derivative = error - lastError[i];
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

void goNeutral() { moveLegsSmooth(NEUTRO_ANGLES); }
void senta()     { moveLegsSmooth(SIT_ANGLES); }
void deitar()    { moveLegsSmooth(LAY_ANGLES); }

int calcServoAngle(float xNeutral, float xTarget, bool invert) {
  float delta = xTarget - xNeutral;
  float percent = delta / STEP_X;
  int angle = 90 + percent * 40;
  if (invert) angle = 180 - angle;
  return constrain(angle, 0, 180);
}

void darPatinha() {
  int target[4] = {curAngle[0], curAngle[1], curAngle[2], curAngle[3]};
  for (int i = 0; i < 2; i++) {
    target[1] = 170; 
    moveLegsSmooth(target);
    delay(200);
    target[1] = 90;
    moveLegsSmooth(target);
    delay(200);
  }
  goNeutral();
}

void walkForward() {
  int target[4];
  target[0] = calcServoAngle(0, -STEP_X, false);
  target[1] = 90; target[2] = 90; target[3] = 90;
  moveLegsSmooth(target); goNeutral();

  target[0] = 90;
  target[1] = calcServoAngle(0, -STEP_X, true);
  moveLegsSmooth(target); goNeutral();

  target[1] = 90;
  target[2] = calcServoAngle(0, -STEP_X, false);
  moveLegsSmooth(target); goNeutral();

  target[2] = 90;
  target[3] = calcServoAngle(0, -STEP_X, true);
  moveLegsSmooth(target); goNeutral();
}

// ======================================================
// FUNÇÕES DE OLHOS
// ======================================================
void reset_eyes() {
  left_eye_width = right_eye_width = ref_eye_width;
  left_eye_height = right_eye_height = ref_eye_height;
  corner_radius = ref_corner_radius;
  left_eye_x = SCREEN_WIDTH/2 - ref_eye_width/2 - ref_space_between_eye/2;
  right_eye_x = SCREEN_WIDTH/2 + ref_eye_width/2 + ref_space_between_eye/2;
  left_eye_y = right_eye_y = SCREEN_HEIGHT/2;
  display_clearDisplay();
}

void draw_eyes() {
  display_fillRoundRect(left_eye_x - left_eye_width/2, left_eye_y - left_eye_height/2,
                        left_eye_width, left_eye_height, corner_radius, COLOR_EYE);
  display_fillRoundRect(right_eye_x - right_eye_width/2, right_eye_y - right_eye_height/2,
                        right_eye_width, right_eye_height, corner_radius, COLOR_EYE);
}

// ----------- ANIMAÇÕES -------------
void blink() {
  reset_eyes();
  for(int i=0;i<2;i++){
    left_eye_height = right_eye_height = 5;
    draw_eyes();
    delay(150);
    reset_eyes();
    draw_eyes();
    delay(150);
  }
}

void wakeup() {
  reset_eyes();
  for(int h=2; h<=ref_eye_height; h+=4){
    left_eye_height = right_eye_height = h;
    corner_radius = (h>ref_corner_radius)? ref_corner_radius : h;
    draw_eyes();
    delay(50);
  }
}

void sleep_eyes() {
  reset_eyes();
  left_eye_height = right_eye_height = 2;
  corner_radius = 0;
  draw_eyes();
}

// ============================
// 😲 SURPRESO
// ============================
void eyes_surprised() {
  reset_eyes();
  // olhos maiores que o normal
  left_eye_height = right_eye_height = ref_eye_height + 20;
  left_eye_width  = right_eye_width  = ref_eye_width + 20;
  corner_radius   = 30;  // bem redondinho
  draw_eyes();
  delay(500);

  // animação: volta devagar para o tamanho normal
  for (int h = ref_eye_height + 20; h >= ref_eye_height; h -= 4) {
    left_eye_height = right_eye_height = h;
    left_eye_width  = right_eye_width  = ref_eye_width + (h - ref_eye_height);
    draw_eyes();
    delay(30);
  }
  reset_eyes();
  draw_eyes();
}

// ============================
// 👀 OLHAR PARA OS LADOS
// dir = -1 esquerda, 1 direita
// ============================
void eyes_lookSide(int dir) {
  reset_eyes();
  left_eye_x += dir * 15;
  right_eye_x += dir * 15;
  draw_eyes();
  delay(300);

  // volta para o centro
  reset_eyes();
  draw_eyes();
}

// ============================
// 😉 PISCADINHA
// eye = 0 pisca esquerdo, 1 pisca direito
// ============================
void eyes_wink(int eye) {
  reset_eyes();
  if (eye == 0) {  // pisca esquerdo
    left_eye_height = 5;
  } else {         // pisca direito
    right_eye_height = 5;
  }
  draw_eyes();
  delay(200);
  reset_eyes();
  draw_eyes();
}


void walkAndBlinkNatural() {
  for (int step = 0; step < 5; step++) {
    walkForward();

    // 30% de chance de piscar depois de andar
    if (random(0, 100) < 30) {
      blink();
    }

    // 10% de chance de dar uma piscadinha dupla
    if (random(0, 100) < 10) {
      blink();
    }

    // delay com leve variação para parecer espontâneo
    int pause = 200 + random(0, 200); 
    delay(pause);
  }
}


// ======================================================
// SETUP & LOOP
// ======================================================
void setup() {
  Serial.begin(115200);

  // Configuração SPI e painel LovyanGFX
  static lgfx::Bus_SPI bus;
  auto cfg = bus.config();
  cfg.spi_mode  = 0;
  cfg.freq_write = 40000000;
  cfg.freq_read  = 16000000;
  cfg.pin_sclk  = 22;
  cfg.pin_mosi  = 21;
  cfg.pin_miso  = -1;
  cfg.pin_dc    = 17;
  cfg.spi_3wire = true;
  bus.config(cfg);

  static lgfx::Panel_GC9A01 panel;
  auto pcfg = panel.config();
  pcfg.pin_cs   = 5;
  pcfg.pin_rst  = 16;
  panel.config(pcfg);

  panel.setBus(&bus);
  tft.setPanel(&panel);
  tft.init();

  reset_eyes();
  draw_eyes();

  for (int i = 0; i < 4; i++) {
    servos[i].attach(S_PIN[i]);
    servos[i].write(90);
  }

  // AÇÕES PARA VÍDEO
  goNeutral();
  wakeup();
  delay(300);
  eyes_lookSide(1);  // olha para direita
  delay(500);
  eyes_lookSide(-1); // olha para esquerda
  delay(500);
  walkAndBlinkNatural();
  delay(300);
  darPatinha();
  eyes_wink(0);  // piscadinha com olho esquerdo
  delay(500);
  senta();
  eyes_surprised();
  delay(500);
  deitar();
  sleep_eyes();
}

void loop() {
  // não repete
}

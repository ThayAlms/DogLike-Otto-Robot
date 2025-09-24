#include <LovyanGFX.hpp>

lgfx::LGFX_Device tft;

// Cores
#define COLOR_BACKGROUND TFT_BLUE // fundo
#define COLOR_EYE        TFT_WHITE // olhos
#define COLOR_PUPIL      TFT_BLACK // pupilas ou detalhes

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

// ----------------------------------------------------------------
// Funções de display
// ----------------------------------------------------------------
void display_clearDisplay() {
  tft.fillScreen(COLOR_BACKGROUND);
}

void display_fillRoundRect(int x,int y,int w,int h,int r,uint16_t color) {
  tft.fillRoundRect(x, y, w, h, r, color);
}

void display_fillTriangle(int x0,int y0,int x1,int y1,int x2,int y2,uint16_t color) {
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

  display_clearDisplay();
}

void draw_eyes() {
  display_fillRoundRect(left_eye_x - left_eye_width/2, left_eye_y - left_eye_height/2,
                        left_eye_width, left_eye_height, corner_radius, COLOR_EYE);
  display_fillRoundRect(right_eye_x - right_eye_width/2, right_eye_y - right_eye_height/2,
                        right_eye_width, right_eye_height, corner_radius, COLOR_EYE);
}

// ----------------------------------------------------------------
// Animações básicas
// ----------------------------------------------------------------
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

void sleep_eyes() {
  reset_eyes();
  left_eye_height = right_eye_height = 2;
  corner_radius = 0;
  draw_eyes();
}

void wakeup() {
  reset_eyes();
  delay(100);
  for(int h=2; h<=ref_eye_height; h+=4){
    left_eye_height = right_eye_height = h;
    corner_radius = (h>ref_corner_radius)? ref_corner_radius : h;
    draw_eyes();
    delay(50);
  }
}

// ----------------------------------------------------------------
// Expressões e movimentos avançados
// ----------------------------------------------------------------
void happy_eye() {
  reset_eyes();
  int steps = 5;               // número de frames da animação
  int eye_offset = ref_eye_height / 2;

  for(int i=0; i<steps; i++) {
    // Redesenha olhos
    draw_eyes();

    // Desenha curva de sorriso (triângulos inclinados)
    display_fillTriangle(
      left_eye_x - left_eye_width/2, left_eye_y + eye_offset,
      left_eye_x + left_eye_width/2, left_eye_y + 5 + eye_offset,
      left_eye_x - left_eye_width/2, left_eye_y + left_eye_height + eye_offset,
      COLOR_PUPIL
    );

    display_fillTriangle(
      right_eye_x + right_eye_width/2, right_eye_y + eye_offset,
      right_eye_x - right_eye_width/2, right_eye_y + 5 + eye_offset,
      right_eye_x + right_eye_width/2, right_eye_y + right_eye_height + eye_offset,
      COLOR_PUPIL
    );

    eye_offset -= 4; // animação subindo o arco
    delay(80);
    reset_eyes();   // prepara para o próximo frame
  }

  // Mantém olhos felizes por um instante
  draw_eyes();
  delay(300);
}


void saccade(int dir_x, int dir_y) {
  reset_eyes();
  left_eye_x += dir_x*8; right_eye_x += dir_x*8;
  left_eye_y += dir_y*6; right_eye_y += dir_y*6;
  draw_eyes();
  delay(150);
  reset_eyes();
  draw_eyes();
}

void move_big_eye(int dir) {
  int steps = 5;      // número de passos suaves
  int step_size = 2;  // pixels por passo

  for(int i=0; i<steps; i++){
    left_eye_x += dir * step_size;
    right_eye_x += dir * step_size;
    draw_eyes();
    delay(50);        // controla velocidade da animação
  }

  reset_eyes();       // volta à posição original
  draw_eyes();
}

// Funções de conveniência
void move_right_big_eye(){ move_big_eye(1); }
void move_left_big_eye(){ move_big_eye(-1); }


// ----------------------------------------------------------------
// Setup e Loop
// ----------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // Configuração SPI
  static lgfx::Bus_SPI bus;
  {
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
  }

  // Configuração painel GC9A01
  static lgfx::Panel_GC9A01 panel;
  {
    auto cfg = panel.config();
    cfg.pin_cs   = 5;
    cfg.pin_rst  = 16;
    cfg.pin_busy = -1;
    panel.config(cfg);
  }

  panel.setBus(&bus);
  tft.setPanel(&panel);
  tft.init();

  reset_eyes();
  draw_eyes();
}

void loop() {
  blink();
  delay(1000);
  sleep_eyes();
  delay(1000);
  wakeup();
  delay(1000);
}

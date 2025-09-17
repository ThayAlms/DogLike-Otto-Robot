#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// Ligações do display
#define TFT_CS   5
#define TFT_DC   17
#define TFT_RST  16
#define TFT_SCLK 22
#define TFT_MOSI 21

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

void sleep() {
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
    delay(50);
    draw_eyes();
    delay(50);
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

void saccade(int dir_x, int dir_y) {
  reset_eyes();
  left_eye_x += dir_x*8; right_eye_x += dir_x*8;
  left_eye_y += dir_y*6; right_eye_y += dir_y*6;
  draw_eyes();
  delay(150);
  reset_eyes();
  draw_eyes();
}

void move_big_eye(int dir){
  reset_eyes();
  for(int i=0;i<2;i++){
    left_eye_x += dir*10; right_eye_x += dir*10;
    draw_eyes();
    delay(100);
  }
  reset_eyes();
}

void move_right_big_eye(){ move_big_eye(1); }
void move_left_big_eye(){ move_big_eye(-1); }

void setup(){
  tft.begin();
  tft.setRotation(0);
  display_clearDisplay();
  reset_eyes();
  draw_eyes();
  delay(1000);
}

void loop(){
  draw_eyes();
  blink();
  sleep();
  reset_eyes();
  draw_eyes();
}
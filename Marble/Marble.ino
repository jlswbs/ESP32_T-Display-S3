// Marble patterns //

#include "TFT_eSPI.h"

#define PIN_LCD_BL    38
#define PIN_LCD_D0    39
#define PIN_LCD_D1    40
#define PIN_LCD_D2    41
#define PIN_LCD_D3    42
#define PIN_LCD_D4    45
#define PIN_LCD_D5    46
#define PIN_LCD_D6    47
#define PIN_LCD_D7    48
#define PIN_LCD_RES   5
#define PIN_LCD_CS    6
#define PIN_LCD_DC    7
#define PIN_LCD_WR    8
#define PIN_LCD_RD    9
#define PIN_BUTTON_1  14
#define PIN_BUTTON_2  0

TFT_eSPI tft = TFT_eSPI();

  #define WIDTH   320
  #define HEIGHT  170
  #define SCR     (WIDTH * HEIGHT)
  #define Imax    4
  #define IImax   4 

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  bool color = false;
  float a = 20.0f;
  float b = 0.02f;  
  float x, y;

  float FX[2][IImax];
  float FY[2][IImax]; 

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for (int II = 0 ; II < IImax ; II++ ){
    FX[0][II] = randomf(0.04f, 0.08f); FX[1][II] = randomf(0.0f, 0.01f);
    FY[0][II] = randomf(0.04f, 0.08f); FY[1][II] = randomf(0.0f, 0.01f) ;
  }

}

void setup(){

  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);

  srand(time(NULL));
  
  tft.begin();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  col = (uint16_t*)ps_malloc(4*SCR);  

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();
  if(digitalRead(PIN_BUTTON_2) == false) color = !color;     

  for (int j = 0; j < HEIGHT; j++) {
    for (int i = 0; i < WIDTH; i++) {

      x = i;
      y = j; 
    
      for (int I = 0 ; I < Imax ; I++ ){
        float dx = 0 ;
        float dy = 0 ;
        for (int II = 0 ; II < IImax ; II++ ){
          dx = a * sinf(y*FX[0][II]) * sinf(y*FX[1][II]) + dx;  
          dy = a * sinf(x*FY[0][II]) * sinf(x*FY[1][II]) + dy;
        }
        x = x + dx;
        y = y + dy;
      }
    
      uint8_t coll = 128.0f + (127.0f * sinf((x+y)*b));
      if(color) col[i+j*WIDTH] = color565(128.0f + (127.0f * sinf(x*b)), coll, 128.0f + (127.0f * sinf(y*b)));
      else col[i+j*WIDTH] = color565(coll, coll, coll);
    }
  }

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

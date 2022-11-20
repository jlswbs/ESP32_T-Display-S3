// Belousov-Zabotinsky reaction //

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
  #define SCL     2

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;

  float *a = NULL;
  float *b = NULL;
  float *c = NULL;

  float adjust = 1.2f;
  bool p = 0, q = 1;
  bool color = false;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  p = 0;
  q = 1;
  adjust = randomf(0.75f, 1.35f);

  for(int i=0; i<SCR*SCL; i++){
    a[i] = randomf(0.0f, 1.0f);
    b[i] = randomf(0.0f, 1.0f);
    c[i] = randomf(0.0f, 1.0f);
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

  a = (float*)ps_malloc(4*SCR*SCL);
  b = (float*)ps_malloc(4*SCR*SCL);
  c = (float*)ps_malloc(4*SCR*SCL);
  col = (uint16_t*)malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();
  if(digitalRead(PIN_BUTTON_2) == false) color = !color;

  for (int y = 0; y < HEIGHT; y++){

    for (int x = 0; x < WIDTH; x++){
  
      float c_a = 0;
      float c_b = 0;
      float c_c = 0;
    
      for (int i = x - 1; i <= x+1; i++) {

        for (int j = y - 1; j <= y+1; j++) {

          c_a += a[(((i+WIDTH)%WIDTH)+((j+HEIGHT)%HEIGHT)*WIDTH)+SCR*p];
          c_b += b[(((i+WIDTH)%WIDTH)+((j+HEIGHT)%HEIGHT)*WIDTH)+SCR*p];
          c_c += c[(((i+WIDTH)%WIDTH)+((j+HEIGHT)%HEIGHT)*WIDTH)+SCR*p];
        }
      }
      
      c_a /= 9.0f;
      c_b /= 9.0f;
      c_c /= 9.0f;

      a[(x+y*WIDTH)+SCR*q] = constrain(c_a + c_a * (adjust * c_b - c_c), 0.0f, 1.0f);
      b[(x+y*WIDTH)+SCR*q] = constrain(c_b + c_b * (c_c - adjust * c_a), 0.0f, 1.0f);
      c[(x+y*WIDTH)+SCR*q] = constrain(c_c + c_c * (c_a - c_b), 0.0f, 1.0f);
    
      uint8_t l = 255.0f * a[(x+y*WIDTH)+SCR*q];
      uint8_t r = 255.0f * b[(x+y*WIDTH)+SCR*q];
      uint8_t s = 255.0f * c[(x+y*WIDTH)+SCR*q];
      uint8_t coll = 255.0f * a[(x+y*WIDTH)+SCR*q];
      if(color) col[x+y*WIDTH] = color565(l, r, s);
      else col[x+y*WIDTH] = color565(coll, coll, coll);
      
    }
  }

  if (p == 0) { p = 1; q = 0; } else { p = 0; q = 1; }
  
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

// Eye candy patterns //

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
#define PIN_BUTTON    14

TFT_eSPI tft = TFT_eSPI();

  #define WIDTH   320
  #define HEIGHT  170
  #define SCR     (WIDTH * HEIGHT)
  #define NUMS    64

uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;

  float *p = NULL;
  float *v = NULL;
  float *a = NULL;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  memset(p, 0, 4*SCR);
  memset(v, 0, 4*SCR);
  memset(a, 0, 4*SCR);
  
  for (int i = 0; i < NUMS; i++) v[(1+esp_random()%(WIDTH-2))+(1+esp_random()%(HEIGHT-2))*WIDTH] = randomf(0.0f, TWO_PI);

}

void setup(){

  pinMode(PIN_BUTTON, INPUT);

  srand(time(NULL));
  
  tft.begin();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  p = (float*)ps_malloc(4*SCR);
  v = (float*)ps_malloc(4*SCR);
  a = (float*)ps_malloc(4*SCR);
  col = (uint16_t*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON) == false) rndrule();

  for (int y = 1; y < HEIGHT-1; y++) {
    for (int x = 1; x < WIDTH-1; x++) {
      a[x+y*WIDTH] = (v[(x-1)+y*WIDTH] + v[(x+1)+y*WIDTH] + v[x+(y-1)*WIDTH] + v[x+(y+1)*WIDTH]) * 0.25f - v[x+y*WIDTH];
    }
  }
  
  for (int i = 0; i < SCR; i++){
    v[i] += a[i];
    p[i] += v[i];
    uint8_t coll = 255 * sinf(p[i]);
    col[i] = color565(coll, coll, coll);
  }

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

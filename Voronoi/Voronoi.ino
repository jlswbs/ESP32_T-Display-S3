// Voronoi distribution (cell noise) //

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
  #define PARTICLES 12  

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float distance(int x1, int y1, int x2, int y2) { return sqrtf(powf(x2 - x1, 2.0f) + powf(y2 - y1, 2.0f)); }

  uint16_t *col = NULL;
  bool color = false;
  float mindist;
  int x[PARTICLES];
  int y[PARTICLES];
  int dx[PARTICLES];
  int dy[PARTICLES];

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for (int i=0; i<PARTICLES; i++) {
    
    x[i] = esp_random()%WIDTH;
    y[i] = esp_random()%HEIGHT;
    dx[i] = esp_random()%8;
    dy[i] = esp_random()%8;
    
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

for (int j=0; j<HEIGHT; j++) {
    
    for (int i=0; i<WIDTH; i++) {
      
      mindist = WIDTH;
      
      for (int p=0; p<PARTICLES; p++) {
        if (distance(x[p], y[p], i, j) < mindist) mindist = distance(x[p], y[p], i, j);
        if (distance(x[p]+WIDTH, y[p], i, j) < mindist) mindist = distance(x[p]+WIDTH, y[p], i, j);
        if (distance(x[p]-WIDTH, y[p], i, j) < mindist) mindist = distance(x[p]-WIDTH, y[p], i, j);
      }
      
      uint8_t coll = 2.0f * mindist;
      if(color) col[i+j*WIDTH] = color565(coll, j, coll);
      else col[i+j*WIDTH] = color565(coll, coll, coll);

    }
  }
  
  for (int p=0; p<PARTICLES; p++) {
    x[p] += dx[p];
    y[p] += dy[p];
    x[p] = x[p] % WIDTH;
    y[p] = y[p] % HEIGHT;
  }

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

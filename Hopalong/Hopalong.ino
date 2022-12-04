// Hopalong chaotic map //

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
  #define ITER    5000 

  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}
  float sign(float a) { return (a == 0.0f) ? 0.0f : (a<0.0f ? -1.0f : 1.0f); }

  uint16_t *col = NULL;
  float x = 1.0f;
  float y = 0.0f;
  float a, b, c;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  x = 1.0f;
  y = 0.0f;
  a = expf(randomf(0.1f, 1.0f) * logf(HEIGHT>>2));
  b = expf(randomf(0.1f, 1.0f) * logf(HEIGHT>>2));
  c = randomf(0.1f, 1.0f) * (HEIGHT>>2);

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

  uint16_t coll = esp_random();
  
  for (int i=0; i<ITER; i++) {
    
    float nx = x;
    float ny = y;
        
    x = ny - 1.0f - sqrtf(fabs(b * (nx - 1.0f) - c)) * sign(nx - 1.0f);
    y = a - (nx - 1.0f);

    int ax = constrain((WIDTH/2) + x, 0, WIDTH);
    int ay = constrain((HEIGHT/2) + y, 0, HEIGHT);
      
    if (ax>0 && ax<WIDTH && ay>0 && ay <HEIGHT) col[ax+ay*WIDTH] = coll;
        
  }

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

// Fizzy 2D cellular automata //

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

uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;

  float *CellVal = NULL;
  uint8_t Calm = 233;
  int CellIndex = 0;
  bool color = false;  

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  CellIndex = 0;
  Calm = 16 + esp_random()%233;
  for (int i = 0; i < SCR; i++) CellVal[i] = randomf(0.0f, 128.0f);

}

void setup(){

  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);

  srand(time(NULL));
  
  tft.begin();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  CellVal = (float*)ps_malloc(4*SCR);
  col = (uint16_t*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();
  if(digitalRead(PIN_BUTTON_2) == false) color = !color;

  for (int i = 0; i < WIDTH; i++) {
    
    for (int j = 0; j < HEIGHT; j++) {

      CellIndex = (CellIndex+1)%SCR;

      uint8_t klimp = (uint8_t)round(CellVal[CellIndex]*4.7f)%100;       
      uint8_t nifna = (uint8_t)round(CellVal[CellIndex]*3.9f)%100;
      uint8_t blugg = (uint8_t)round(CellVal[CellIndex]*5.5f)%100;

      if(color) col[i+j*WIDTH] = color565(klimp<<1, nifna<<1, blugg<<1);
      else col[i+j*WIDTH] = color565(klimp<<1, klimp<<1, klimp<<1);      

      int below      = (CellIndex+1)%SCR;
      int above      = (CellIndex+SCR-1)%SCR;
      int left       = (CellIndex+SCR-HEIGHT)%SCR;
      int right      = (CellIndex+HEIGHT)%SCR;
      int aboveright = ((CellIndex-1) + HEIGHT + SCR)%SCR;
      int aboveleft  = ((CellIndex-1) - HEIGHT + SCR)%SCR;
      int belowright = ((CellIndex+1) + HEIGHT + SCR)%SCR;
      int belowleft  = ((CellIndex+1) - HEIGHT + SCR)%SCR;

      float NeighbourMix = powf((CellVal[left]*CellVal[right]*CellVal[above]*CellVal[below]*CellVal[belowleft]*CellVal[belowright]*CellVal[aboveleft]*CellVal[aboveright]),0.125f);
      CellVal[CellIndex] = fmod((sqrtf(CellVal[CellIndex]*NeighbourMix))+0.5f, Calm);

    }

  }

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

// Multi Langton's ant cellular automata //

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
#define PIN_POWER_ON  15
#define PIN_BUTTON_1  14
#define PIN_BUTTON_2  0

TFT_eSPI tft = TFT_eSPI();

  #define WIDTH   320
  #define HEIGHT  170
  #define SCR     (WIDTH * HEIGHT)
  #define ITER    120
  #define NUMANTS 6  

  uint16_t *col = NULL;
  uint16_t coll[NUMANTS];
  int x[NUMANTS];
  int y[NUMANTS];
  int antsdir[NUMANTS];

void rndrule(){

  memset((uint8_t *) col, 0, 4*SCR);

  for(int i = 0; i < NUMANTS; i++){
  
    x[i] = esp_random()%WIDTH;
    y[i] = esp_random()%HEIGHT;
    antsdir[i] = esp_random()%4;
    coll[i] = esp_random();
    
  }
  
}

void setup(){

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
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

  for(int k = 0; k < ITER; k++){
  
    for(int i = 0; i < NUMANTS; i++){
    
      if (col[x[i]+WIDTH*y[i]] > TFT_BLACK){ antsdir[i] = antsdir[i] - 1; col[x[i]+WIDTH*y[i]] = TFT_BLACK; }
      else { antsdir[i] = antsdir[i] + 1; col[x[i]+WIDTH*y[i]] = coll[i]; }

      if (antsdir[i] > 3) antsdir[i] = 0;   
      if (antsdir[i] < 0) antsdir[i] = 3;   
    
      if (antsdir[i] == 0) x[i] = x[i] - 1;
      if (antsdir[i] == 1) y[i] = y[i] + 1;
      if (antsdir[i] == 2) x[i] = x[i] + 1;
      if (antsdir[i] == 3) y[i] = y[i] - 1;
    
      if (x[i] > WIDTH-1) x[i] = 0;
      if (x[i] < 0) x[i] = WIDTH-1;
      if (y[i] > HEIGHT-1) y[i] = 0;
      if (y[i] < 0) y[i] = HEIGHT-1;
    
    }
    
  }
    
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
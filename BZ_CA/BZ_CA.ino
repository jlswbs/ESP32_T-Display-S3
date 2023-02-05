// Belousov-Zhabotinsky reaction cellular automata //

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

  uint16_t *col = NULL;
  uint16_t image;
  uint8_t *cells = NULL;
  uint8_t *nextcells = NULL;
  int dir[2][4] = {{0, 2, 0, -2},{-2, 0, 2, 0}};

void rndrule(){

  memset((uint8_t *) col, 0, 4*SCR);
  memset((uint8_t *) cells, 0, 4*SCR);
  memset((uint8_t *) nextcells, 0, 4*SCR);

  cells[(esp_random()%WIDTH)+(esp_random()%HEIGHT)*WIDTH] = 1;  
  
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

  cells = (uint8_t*)ps_malloc(4*SCR);
  nextcells = (uint8_t*)ps_malloc(4*SCR);
  col = (uint16_t*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();

  for(int y = 0; y < HEIGHT; y=y+2){
    
    for(int x = 0; x < WIDTH; x=x+2){
      
      if(cells[x+y*WIDTH] == 0) image = TFT_BLACK;
      else if(cells[x+y*WIDTH] == 1) image = TFT_RED;
      else image = TFT_WHITE;
      col[x+y*WIDTH] = image;
      
    }
  }

  for(int y = 0; y < HEIGHT; y=y+2){
    
    for(int x = 0; x < WIDTH; x=x+2){
      
      if(cells[x+y*WIDTH] == 1) nextcells[x+y*WIDTH] = 2;
      else if(cells[x+y*WIDTH] == 2) nextcells[x+y*WIDTH] = 0;
      else {
        
        nextcells[x+y*WIDTH] = 0;
        
        for(int k = 0; k < 4; k++){
          int dx = x + dir[0][k];
          int dy = y + dir[1][k];
          if(0 <= dx && dx < WIDTH && 0 <= dy && dy < HEIGHT && cells[dx+dy*WIDTH] == 1) nextcells[x+y*WIDTH] = 1;
        }
      
      }
    }
  }

  memcpy(cells, nextcells, 4*SCR);

  cells[(esp_random()%WIDTH)+(esp_random()%HEIGHT)*WIDTH] = 1;
  
  delayMicroseconds(240);  
  
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
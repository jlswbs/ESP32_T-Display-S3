// Conway's game of life cellular automata //

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
  bool *grid = NULL;
  bool current;

void rndrule(){

  current = false;  

  memset((uint16_t *) col, 0, 4*SCR);
  memset((bool *) grid, 0, 4*(2*SCR));   
    
  for (int i = 0; i < SCR; i++) grid[i] = esp_random()%2; 
  
}

void rungrid(){
  
  int x, y;
  int count;
  int value = 0;
  bool new_grid;

  new_grid = 1 - current;
  
  for (y = 0; y < HEIGHT; y++) {
  
    for (x = 0; x < WIDTH; x++) {
      
      count = neighbours(x, y);
      
      if (count < 2 || count > 3) { value = 0; }
      else if (count == 3) { value = 3; }
      else { value = grid[(current*SCR)+(x+y*WIDTH)]; }
    
      grid[(new_grid*SCR)+(x+y*WIDTH)] = value;

      if(grid[(current*SCR)+(x+y*WIDTH)]) col[x+y*WIDTH] = TFT_WHITE;
      else col[x+y*WIDTH] = TFT_BLACK;
    
    }
  }
  
  current = new_grid;
}


int neighbours(int x, int y){
  
  int i, j;
  int result = 0;

  x--;
  y--;
  
  for (i = 0; i < 3; i++) {
  
    if (y < 0 || y > (HEIGHT - 1)) continue;

    for (j = 0; j < 3; j++) {
      if (x < 0 || x > (WIDTH - 1)) continue;
      if (i==1 && j == 1) { x++; continue; }
      if (grid[(current*SCR)+(x+y*WIDTH)]) result++;
      x++;
    }
    y++;
    x -= 3;
  }
  
  return result;
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
  grid = (bool*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();

  rungrid();
  
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
// Turmites cellular automata //

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
  #define ITER    240

  uint16_t *col = NULL;
  uint8_t *world = NULL;
  int posx, posy;
  int oldposx, oldposy;
  int state;
  int dir;
  int last_filled;
  int current_col;
  int next_col[4][4];
  int next_state[4][4];
  int directions[4][4];

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  memset((uint8_t *) world, 0, 4*SCR);  

  state = esp_random()%4;
  dir = 0;
  posx = WIDTH/2;
  posy = HEIGHT/2;
  
  for(int j=0; j<4; j++){   
    for(int i=0; i<4; i++){         
      next_col[i][j] = esp_random()%4;
      next_state[i][j] = esp_random()%4;
      directions[i][j] = esp_random()%8;
    }   
  }

  world[posx+posy*WIDTH] = esp_random()%4;
  
}

void move_turmite(){
  
  int cols = world[posx+posy*WIDTH];
  
  oldposx = posx;
  oldposy = posy;
  current_col = next_col[cols][state];
  world[posx+posy*WIDTH] = next_col[cols][state];
  state = next_state[cols][state];    

  dir = (dir + directions[cols][state]) % 8;

  switch(dir){
    case 0: posy--; break;
    case 1: posy--; posx++; break;
    case 2: posx++; break;
    case 3: posx++; posy++; break;
    case 4: posy++; break;
    case 5: posy++; posx--; break;
    case 6: posx--; break;
    case 7: posx--; posy--; break;
  }

  if(posy < 0) posy = HEIGHT-1;
  if(posy >= HEIGHT) posy = 0;
  if(posx < 0) posx = WIDTH-1;
  if(posx >= WIDTH) posx=0;
  
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
  world = (uint8_t*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();

  for (int i = 0; i < ITER; i++) {

    move_turmite();

    switch(current_col){
      case 0: col[oldposx + oldposy * WIDTH] = TFT_RED; break;
      case 1: col[oldposx + oldposy * WIDTH] = TFT_GREEN; break;
      case 2: col[oldposx + oldposy * WIDTH] = TFT_BLUE; break;
      case 3: col[oldposx + oldposy * WIDTH] = TFT_WHITE; break;
    }
    
  }
  
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
// Wolfram 2D cellular automata //

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
  bool *state = NULL;
  bool *newstate = NULL;
  bool rules[10];

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  
  for(int i = 0; i < 10; i++) rules[i] = esp_random()%2;

  memset(newstate, 0, SCR);
  memset(state, 0, SCR);
  
  state[(WIDTH/2)+(HEIGHT/2)*WIDTH] = 1;
  state[(WIDTH/2)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+(HEIGHT/2)*WIDTH] = 1;
  
}

uint8_t neighbors(int i) {

  uint16_t x = i % WIDTH;
  uint16_t y = i / WIDTH;
  uint8_t result = 0;

  if(y > 0 && state[x+(y-1)*WIDTH] == 1) result = result + 1;
  if(x > 0 && state[(x-1)+y*WIDTH] == 1) result = result + 1;
  if(x < WIDTH-1 && state[(x+1)+y*WIDTH] == 1) result = result + 1;
  if(y < HEIGHT-1 && state[x+(y+1)*WIDTH] == 1) result = result + 1;
  
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
  state = (bool*)ps_malloc(SCR);
  newstate = (bool*)ps_malloc(SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();
    
  for(int i = 0; i < SCR; i++){
    
    uint8_t totalNeighbors = neighbors(i);
            
    if(state[i] == 0 && totalNeighbors == 0)      {newstate[i] = rules[0]; col[i] = TFT_WHITE;}
    else if(state[i] == 1 && totalNeighbors == 0) {newstate[i] = rules[1]; col[i] = TFT_RED;}
    else if(state[i] == 0 && totalNeighbors == 1) {newstate[i] = rules[2]; col[i] = TFT_GREEN;}
    else if(state[i] == 1 && totalNeighbors == 1) {newstate[i] = rules[3]; col[i] = TFT_BLUE;}
    else if(state[i] == 0 && totalNeighbors == 2) {newstate[i] = rules[4]; col[i] = TFT_YELLOW;}
    else if(state[i] == 1 && totalNeighbors == 2) {newstate[i] = rules[5]; col[i] = TFT_BLUE;}
    else if(state[i] == 0 && totalNeighbors == 3) {newstate[i] = rules[6]; col[i] = TFT_MAGENTA;}
    else if(state[i] == 1 && totalNeighbors == 3) {newstate[i] = rules[7]; col[i] = TFT_CYAN;}
    else if(state[i] == 0 && totalNeighbors == 4) {newstate[i] = rules[8]; col[i] = TFT_RED;}
    else if(state[i] == 1 && totalNeighbors == 4) {newstate[i] = rules[9]; col[i] = TFT_BLACK;}
      
  }
 
  memcpy(state, newstate, SCR);
    
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
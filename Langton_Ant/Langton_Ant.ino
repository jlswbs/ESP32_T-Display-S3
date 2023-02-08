// Langton's ant cellular automata //

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
  bool *state = NULL;
  uint16_t image;
  int antLoc[2];
  int antDirection;

void rndrule(){

  memset((uint8_t *) col, 0, 4*SCR);
  memset((bool *) state, 0, 4*SCR);  
  antDirection = 1 + esp_random()%4;
  antLoc[0] = esp_random()%WIDTH;
  antLoc[1] = esp_random()%HEIGHT;
  
}

void turnLeft(){

  if (antDirection > 1) antDirection--;
  else antDirection = 4;

}

void turnRight(){

  if (antDirection < 4) antDirection++;
  else antDirection = 1;

}

void moveForward(){

  if (antDirection == 1) antLoc[0]--;
  if (antDirection == 2) antLoc[1]++;
  if (antDirection == 3) antLoc[0]++;
  if (antDirection == 4) antLoc[1]--;

  if (antLoc[0] < 0) antLoc[0] = WIDTH-1;
  if (antLoc[0] > WIDTH-1) antLoc[0] = 0;
  if (antLoc[1] < 0) antLoc[1] = HEIGHT-1;
  if (antLoc[1] > HEIGHT-1) antLoc[1] = 0;

}

void updateScene(){

  moveForward();

  if (state[antLoc[0]+antLoc[1]*WIDTH] == 0){
    state[antLoc[0]+antLoc[1]*WIDTH] = 1;
    turnRight();
  } else {
    state[antLoc[0]+antLoc[1]*WIDTH] = 0;
    turnLeft();
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
  state = (bool*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();

  for(int i = 0; i < ITER; i++) updateScene();

  for (int i = 0; i < SCR; i++) {
 
    if(state[i] == 1) image = TFT_WHITE;
    else image = TFT_BLACK;
      
    col[i] = image;

  }
  
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
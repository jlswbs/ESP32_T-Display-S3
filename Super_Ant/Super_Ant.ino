// Super Langton's ant cellular automata //

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
  #define ITER    320
  #define LENGHT  32

  uint16_t *col = NULL;
  uint16_t *state = NULL;
  uint16_t antX = WIDTH/2;
  uint16_t antY = HEIGHT/2;
  uint8_t direction;
  uint8_t stateCount;
  bool type[LENGHT];
  uint16_t stateCols[LENGHT];

void rndrule(){

  memset((uint8_t *) col, 0, 4*SCR);
  memset((bool *) state, 0, 4*SCR);  

  antX = WIDTH/2;
  antY = HEIGHT/2;
  stateCount = 2 + esp_random()%(LENGHT-2);
  direction = esp_random()%4;
  for(int i = 0; i < stateCount; i++) type[i] = esp_random()%2;
  for(int i = 0; i < stateCount; i++) stateCols[i] = esp_random();
  
}

void turn(int angle){
  
  if(angle == 0){
    if(direction == 0){
      direction = 3;
    } else {
      direction--;
    }
  } else {
    if(direction == 3){
      direction = 0;
    } else {
      direction++;
    }
  }
}

void move(){
  
  if(antY == 0 && direction == 0){
    antY = HEIGHT-1;
  } else {
    if(direction == 0 ){
      antY--;
    }
  }
  if(antX == WIDTH-1 && direction == 1){
    antX = 0;
  } else {
    if(direction == 1){
      antX++;
    }
  }
  if(antY == HEIGHT-1 && direction == 2){
   antY = 0; 
  } else {
    if(direction == 2){
      antY++;
    }
  }
  if(antX == 0 && direction == 3){
    antX = WIDTH-1;
  } else {
    if(direction == 3){
      antX--;
    }
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
  state = (uint16_t*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();

  for(int i = 0; i < ITER; i++) {

    move();
    turn(type[(state[antX+antY*WIDTH]%stateCount)]);
    state[antX+antY*WIDTH]++;
    col[antX+antY*WIDTH] = stateCols[(state[antX+antY*WIDTH]%stateCount)];

  }
  
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
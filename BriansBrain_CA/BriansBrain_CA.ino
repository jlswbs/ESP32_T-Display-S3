// Brian's brain cellular automata //

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
  #define DENSITY     7
  #define READY       0
  #define REFRACTORY  1
  #define FIRING      2

  uint16_t *col = NULL;
  uint8_t *world = NULL;
  uint8_t *temp = NULL;

uint8_t weighted_randint(int true_weight){
  
    int choice = esp_random() % 10;
    
    if (choice > true_weight) return 1;
    else return 0;
}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  memset((uint8_t *) temp, 0, 4*SCR);

  for (int i = 0; i < SCR; i++){
    int r = weighted_randint(DENSITY);
    if (r == 1) world[i] = FIRING;
    else world[i] = READY;
  }
  
}

uint8_t count_neighbours(uint8_t world[SCR], int x_pos, int y_pos){
  
    int x, y, cx, cy, cell;
    int count = 0;

    for (y = -1; y < 2; y++) {
        for (x = -1; x < 2; x++) {
            cx = x_pos + x;
            cy = y_pos + y;
            if ( (0 <= cx && cx < WIDTH) && (0 <= cy && cy < HEIGHT)) {
                cell = world[(x_pos + x) + (y_pos + y) * WIDTH];
                if (cell == FIRING) count ++;
            }
        }
    }
  return count;
}


void apply_rules(uint8_t world[SCR]){
  
  int cell, neighbours;

  memcpy(temp, world, 4*SCR);

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++){
      cell = temp[x+y*WIDTH];          
      if (cell == READY) {
        neighbours = count_neighbours(temp, x, y);
        if (neighbours == 2) world[x+y*WIDTH] = FIRING; }
      else if (cell == FIRING) world[x+y*WIDTH] = REFRACTORY;
      else world[x+y*WIDTH] = READY;
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
  world = (uint8_t*)ps_malloc(4*SCR);
  temp = (uint8_t*)ps_malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();

  apply_rules(world);

  for (int i = 0; i < SCR; i++) {
    if (world[i] == FIRING) col[i] = TFT_BLUE;    
    else if (world[i] == REFRACTORY) col[i] = TFT_WHITE;
    else col[i] = TFT_BLACK; 
  }
    
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
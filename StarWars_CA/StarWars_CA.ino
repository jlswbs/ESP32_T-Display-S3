// Star-Wars cellular automata //

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
  #define ALIVE   3
  #define DEATH_1 2
  #define DEATH_2 1
  #define DEAD    0

  uint16_t *col = NULL;
  uint8_t *current = NULL;
  uint8_t *next = NULL;
  uint8_t *alive_counts = NULL;
  uint8_t *temp = NULL;  

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  memset((uint8_t *) next, 0, 4*SCR);
  memset((uint8_t *) alive_counts, 0, 4*SCR);  
  memset((uint8_t *) temp, 0, 4*SCR);

  for (int i = 0; i < SCR; i++) current[i] = (esp_random()%100) < 20 ? ALIVE : DEAD;
  
}

void step(){

  for (int y = 0; y < HEIGHT; y++) {
  
    for (int x = 0; x < WIDTH; x++) {  
    
      int count = 0;
      int next_val;
    
      int mx = WIDTH-1;
      int my = HEIGHT-1;
    
      int self = current[x+y*WIDTH];
    
      for (int nx = x-1; nx <= x+1; nx++) {
  
        for (int ny = y-1; ny <= y+1; ny++) {
    
          if (nx == x && ny == y) continue;     
          if (current[(wrap(nx, mx))+(wrap(ny, my))*WIDTH] == ALIVE) count++;
      
        }   
      }  

    int neighbors = count;
    
    if (self == ALIVE) next_val = ((neighbors == 3) || (neighbors == 4) || (neighbors == 5)) ? ALIVE : DEATH_1;
  
    else if (self == DEAD) next_val = (neighbors == 2) ? ALIVE : DEAD;
  
    else next_val = self-1;
   
    next[x+y*WIDTH] = next_val;
  
    if (next_val == ALIVE) alive_counts[x+y*WIDTH] = min(alive_counts[x+y*WIDTH]+1, 100);
    else if (next_val == DEAD) alive_counts[x+y*WIDTH] = 0;
    
    }
  }
  
  memcpy(temp, current, 4*SCR);
  memcpy(current, next, 4*SCR);
  memcpy(next, temp, 4*SCR);

}
  
int wrap(int v, int m){

    if (v < 0) return v + m;
    else if (v >= m) return v - m;
    else return v;
}

void draw_type(int min_alive, int max_alive, uint16_t color){

  uint16_t coll;
       
  for (int i = 0; i < SCR; i++) {
   
    int self = current[i];
    if (self == DEAD) continue;
    int alive = alive_counts[i];
    if (alive < min_alive || alive > max_alive) continue;
    if (self == ALIVE) coll = color;
    else if (self == DEATH_1) coll = color>>1;
    else if (self == DEATH_2) coll = TFT_BLACK;

    col[i] = coll;

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
  current = (uint8_t*)ps_malloc(4*SCR);
  next = (uint8_t*)ps_malloc(4*SCR);
  alive_counts = (uint8_t*)ps_malloc(4*SCR);
  temp = (uint8_t*)ps_malloc(4*SCR);  

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();

  step();

  draw_type(50, 100, TFT_RED);
  draw_type(2, 49, TFT_BLUE);
  draw_type(0, 1, TFT_WHITE);
    
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}
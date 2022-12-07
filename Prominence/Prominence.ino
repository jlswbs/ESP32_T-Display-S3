// Prominence math patterns //

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
  #define N       1000
  #define M       5   

  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  uint16_t coll = esp_random();
  float h = 0.5f;
  float ds = -1.0f;
  float X[N];
  float Y[N];
  float T[M];
  float dx, dy, a, s, k;
  int ct;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for (int I = 0 ; I < N ; I++){
    X[I] = randomf(0.0f , WIDTH);
    Y[I] = WIDTH;
  }
  
  for (int II = 0 ; II < M ; II++) T[II] = randomf(10.0f, 40.0f);

}

void setup(){

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

  if(ct == 10) {
    
    coll = esp_random();
    ct = 0;
    
  }    

  for (int I = 0; I < N; I++){

    for (int II = 0; II < M; II++){
      
      a = 2.0f * PI * II / M;
      k = (X[I] * cosf(a)) - ((Y[I]+s) * sinf(a));
      k = h * sinf(k/T[II]);
      float kdx = -k * sinf(-a);
      float kdy = +k * cosf(-a);
      dx = dx + kdx;
      dy = dy + kdy;
      
    }
    
    X[I] = X[I] + dx;
    Y[I] = Y[I] + dy;
    dx = 0;
    dy = 0;
    
    if(X[I] < 0 || X[I] > WIDTH || Y[I] > HEIGHT || Y[I] < 0 || randomf(0.0f, 1000.0f) < 1) {
      
      Y[I] = 0.0f;
      X[I] = randomf(0.0f, WIDTH);   
    
    }
    
    a = PI * X[I] / WIDTH;
    k = Y[I] + 10.0f;

    int ax = (WIDTH/2) + (k * sinf(a));
    int bx = (WIDTH/2) - (k * sinf(a));    
    int ay = (HEIGHT/2) + (k * cosf(a));
    
    if(ax>0 && ax<WIDTH && ay>0 && ay<HEIGHT) {      
    
      col[ax + ay * WIDTH] = coll;
      col[bx + ay * WIDTH] = coll;

    }
  
  }
  
  s = s + ds;
  ct++;

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

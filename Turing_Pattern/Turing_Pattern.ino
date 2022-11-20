// Turing patterns //

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
  #define SCL     4

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;

  float *pat = NULL;
  float *pnew = NULL;
  float *pmedian = NULL;
  float *prange = NULL;
  float *pvar = NULL;

  uint8_t lim = 128;
  uint8_t dirs = 9;
  uint8_t patt = 0;
  bool color = false;
  uint16_t cols;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  lim = 64 + esp_random()%128;
  dirs = 6 + esp_random()%5;
  patt = esp_random()%5;

  for(int i=0; i<SCR; i++) pat[i] = randomf(0.0f, 255.0f);

}

void setup(){

  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);

  srand(time(NULL));
  
  tft.begin();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  pat = (float*)ps_malloc(4*SCR);
  pnew = (float*)ps_malloc(4*SCR);
  pmedian = (float*)ps_malloc(4*SCR*SCL);
  prange = (float*)ps_malloc(4*SCR*SCL);
  pvar = (float*)ps_malloc(4*SCR*SCL);
  col = (uint16_t*)malloc(4*SCR);

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();
  if(digitalRead(PIN_BUTTON_2) == false) color = !color;

  float R = randomf(0, TWO_PI);
  
  memcpy(pnew, pat, 4*SCR);
  memset(pmedian, 0, 4*SCR*SCL);
  memset(prange, 0, 4*SCR*SCL);
  memset(pvar, 0, 4*SCR*SCL);
  
  for(int i=0; i<SCL; i++){

    float d = (2<<i);
  
    for(int j=0; j<dirs; j++){
          
      float dir = j * TWO_PI / dirs + R;
      int dx = (d * cosf(dir));
      int dy = (d * sinf(dir));
           
      for(int l=0; l<SCR; l++){
        int x1 = l%WIDTH + dx, y1 = l/WIDTH + dy;
        if(x1<0) x1 = WIDTH-1-(-x1-1); if(x1>=WIDTH) x1 = x1%WIDTH;
        if(y1<0) y1 = HEIGHT-1-(-y1-1); if(y1>=HEIGHT) y1 = y1%HEIGHT;
        pmedian[l+SCR*i] += pat[x1+y1*WIDTH] / dirs;
      }
    }

    for(int j=0; j<dirs; j++){

      float dir = j * TWO_PI / dirs + R;
      int dx = (d * cosf(dir));
      int dy = (d * sinf(dir));
    
      for(int l=0; l<SCR; l++){
        
        int x1 = l%WIDTH + dx, y1 = l/WIDTH + dy;
        if(x1<0) x1 = WIDTH-1-(-x1-1); if(x1>=WIDTH) x1 = x1%WIDTH;
        if(y1<0) y1 = HEIGHT-1-(-y1-1); if(y1>=HEIGHT) y1 = y1%HEIGHT;
        pvar[l+SCR*i] += fabs(pat[x1+y1*WIDTH] - pmedian[l+SCR*i]) / dirs;
        prange[l+SCR*i] += pat[x1+y1*WIDTH] > lim ? +1.0f : -1.0f;
      
      }
    }
  }
  
  for(int l=0; l<SCR; l++){
    
    int imin = 0, imax = SCL;
    float vmin = MAXFLOAT;
    float vmax = -MAXFLOAT;
    
    for(int i=0; i<SCL; i++){
      if(pvar[l+SCR*i] <= vmin) { vmin = pvar[l+SCR*i]; imin = i; }
      if(pvar[l+SCR*i] >= vmax) { vmax = pvar[l+SCR*i]; imax = i; }
    }

    switch(patt){
      case 0: for(int i=0; i<=imin; i++)    pnew[l] += prange[l+SCR*i]; break;
      case 1: for(int i=0; i<=imax; i++)    pnew[l] += prange[l+SCR*i]; break;
      case 2: for(int i=imin; i<=imax; i++) pnew[l] += prange[l+SCR*i]; break;
      case 3: for(int i=imin; i<=imax; i++) pnew[l] += prange[l+SCR*i] + pvar[l+SCR*i] / 2.0f; break;
      case 4: for(int i=imin; i<=imax; i++) pnew[l] += prange[l+SCR*i] + pmedian[l+SCR*i] / 2.0f; break;
    }
      
  }

  float vmin = MAXFLOAT;
  float vmax = -MAXFLOAT;

  for(int i=0; i<SCR; i++){
    vmin = fmin(vmin, pnew[i]);
    vmax = fmax(vmax, pnew[i]);     
  }

  float dv = vmax - vmin;
      
  for (int i = 0; i < SCR; i++){
    
    pat[i] = (pnew[i] - vmin) * 255.0f / dv;
    uint8_t coll = pat[i];
    if(color) cols = color565(coll<<1, coll<<2, coll<<3);
    else cols = color565(coll, coll, coll);
    col[i] = cols;

  }
  
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

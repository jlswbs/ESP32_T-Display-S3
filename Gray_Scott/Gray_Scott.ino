// Gray-Scott reaction diffusion //

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
  #define ITER    48   

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  bool color = false; 
  float diffU = 0.16f;
  float diffV = 0.08f;
  float paramF = 0.035f;
  float paramK =  0.06f;
  float *gridU = NULL;  
  float *gridV = NULL;
  float *dU = NULL;
  float *dV = NULL;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  diffU = randomf(0.0999f, 0.1999f);
  diffV = randomf(0.0749f, 0.0849f);
  paramF = randomf(0.0299f, 0.0399f);
  paramK = randomf(0.0549f, 0.0649f);

  int seed = esp_random() % (SCR>>4);

  for(int i = 0 ; i < SCR ; i++) {
    
    int px = (i % WIDTH) - (WIDTH / 2);
    int py = (i / HEIGHT) - (WIDTH / 2);

    if(px*px + py*py < seed) {
      gridU[i] = 0.5f * randomf(0.0f, 2.0f);
      gridV[i] = 0.25f * randomf(0.0f, 2.0f);
    } else {
      gridU[i] = 1.0f;
      gridV[i] = 0.0f;
    }
  
  }

}

void setup(){

  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);

  srand(time(NULL));
  
  tft.begin();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  gridU = (float*)ps_malloc(4*SCR);
  gridV = (float*)ps_malloc(4*SCR);
  dU = (float*)ps_malloc(4*SCR);
  dV = (float*)ps_malloc(4*SCR);
  col = (uint16_t*)ps_malloc(4*SCR);  

  rndrule();

}

void timestep(float F, float K, float diffU, float diffV) {

  for (int j = 1; j < HEIGHT-1; j++) {
    for (int i = 1; i < WIDTH-1; i++) {
            
      float u = gridU[i+j*WIDTH];
      float v = gridV[i+j*WIDTH];          
      float uvv = u * v * v;   
      float lapU = (gridU[(i-1)+j*WIDTH] + gridU[(i+1)+j*WIDTH] + gridU[i+(j-1)*WIDTH] + gridU[i+(j+1)*WIDTH] - 4.0f * u);
      float lapV = (gridV[(i-1)+j*WIDTH] + gridV[(i+1)+j*WIDTH] + gridV[i+(j-1)*WIDTH] + gridV[i+(j+1)*WIDTH] - 4.0f * v);
          
      dU[i+j*WIDTH] = diffU * lapU - uvv + F * (1.0f-u);
      dV[i+j*WIDTH] = diffV * lapV + uvv - (K+F) * v;
      gridU[i+j*WIDTH] += dU[i+j*WIDTH];
      gridV[i+j*WIDTH] += dV[i+j*WIDTH];      
          
    }
  }

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();
  if(digitalRead(PIN_BUTTON_2) == false) color = !color;

  for (int k = 0; k < ITER; k++) timestep(paramF, paramK, diffU, diffV);

  for(int i = 0; i < SCR; i++){     
    uint8_t coll = 255.0f * sinf(gridU[i]);
    if(color) col[i] = color565(coll<<1, coll<<2, coll<<3);
    else col[i] = color565(coll, coll, coll);
  }

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

// FitzHugh-Nagumo reaction diffusion //

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

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  bool color = false; 
  float reactionRate = 0.2f;
  float diffusionRate = 0.01f;
  float kRate = 0.4f;
  float fRate = 0.09f;  
  float *gridU = NULL;  
  float *gridV = NULL;
  float *gridNext = NULL;
  float *diffRateUYarr = NULL;
  float *diffRateUXarr = NULL;
  float *farr = NULL;
  float *karr = NULL;
  float *temp = NULL;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  diffusionRate = randomf(0.01f, 0.05f);
  kRate = randomf(0.1f, 0.5f);
  fRate = randomf(0.04f, 0.09f);
  
  for(int i=0;i<SCR;++i){
      
    gridU[i] = 0.5f + randomf(-0.01f, 0.01f);
    gridV[i] = 0.25f + randomf(-0.01f, 0.01f);

  }

  setupF();
  setupK();

  for(int i=0;i<SCR;++i){
    
    diffRateUYarr[i] = randomf(0.03f, 0.05f);
    diffRateUXarr[i] = randomf(0.03f, 0.05f);
      
  } 

}

void diffusionV(){
  
  for(int i=0;i<WIDTH;++i){
    for(int j=0;j<HEIGHT;++j){

      gridNext[i+j*WIDTH] = gridV[i+j*WIDTH]+diffusionRate*4.0f*(gridV[((i-1+WIDTH)%WIDTH)+j*WIDTH]+gridV[((i+1)%WIDTH)+j*WIDTH]+gridV[i+((j-1+HEIGHT)%HEIGHT)*WIDTH]+gridV[i+((j+1)%HEIGHT)*WIDTH]-4.0f*gridV[i+j*WIDTH]);
    
    }
  }
  
  memcpy(temp, gridV, 4*SCR);
  memcpy(gridV, gridNext, 4*SCR); 
  memcpy(gridNext, temp, 4*SCR);
  
}

void diffusionU(){
  
  for(int i=0;i<WIDTH;++i){
    for(int j=0;j<HEIGHT;++j){

      gridNext[i+j*WIDTH] = gridU[i+j*WIDTH]+4.0f*(diffRateUXarr[i+j*WIDTH]*(gridU[((i-1+WIDTH)%WIDTH)+j*WIDTH]+gridU[((i+1)%WIDTH)+j*WIDTH]-2.0f*gridU[i+j*WIDTH])+diffRateUYarr[i+j*WIDTH]*(gridU[i+((j-1+HEIGHT)%HEIGHT)*WIDTH]+gridU[i+((j+1)%HEIGHT)*WIDTH]-2.0f*gridU[i+j*WIDTH]));
    
    }
  }
  
  memcpy(temp, gridU, 4*SCR);
  memcpy(gridU, gridNext, 4*SCR); 
  memcpy(gridNext, temp, 4*SCR);
  
}

void reaction(){

  for(int i=0;i<SCR;++i){

    gridU[i] = gridU[i]+4.0f*(reactionRate*(gridU[i]-gridU[i]*gridU[i]*gridU[i]-gridV[i]+karr[i]));
    gridV[i] = gridV[i]+4.0f*(reactionRate*farr[i]*(gridU[i]-gridV[i]));
    
  }

}

void setupF(){
  
  for(int i=0;i<WIDTH;++i){
  
    for(int j=0;j<HEIGHT;++j) farr[i+j*WIDTH] = 0.01f + i * fRate / WIDTH;
  
  }

}

void setupK(){

  for(int i=0;i<WIDTH;++i){
  
    for(int j=0;j<HEIGHT;++j) karr[i+j*WIDTH] = 0.06f + j * kRate / HEIGHT;
    
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
  gridNext = (float*)ps_malloc(4*SCR);
  diffRateUYarr = (float*)ps_malloc(4*SCR);
  diffRateUXarr = (float*)ps_malloc(4*SCR);
  farr = (float*)ps_malloc(4*SCR);
  karr = (float*)ps_malloc(4*SCR);
  temp = (float*)ps_malloc(4*SCR);  
  col = (uint16_t*)ps_malloc(4*SCR);  

  rndrule();

}

void loop() {

  if(digitalRead(PIN_BUTTON_1) == false) rndrule();
  if(digitalRead(PIN_BUTTON_2) == false) color = !color;

  diffusionU();
  diffusionV();
  reaction();

  for(int i = 0; i < SCR; i++){     
    uint8_t coll = 255.0f * gridU[i];
    uint8_t colo = 255.0f * gridV[i];    
    if(color) col[i] = color565(colo, coll, 255-colo);
    else col[i] = color565(coll, coll, coll);
  }

  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)col);

}

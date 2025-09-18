// https://learn.adafruit.com/adafruit-gfx-graphics-library

#include "tftx.h"

#include <Arduino.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>	
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <SPI.h>



#define TFT_WIDTH    160
#define TFT_HEIGHT   128
#define MENU_HEIGHT  16
#define TOP_BOX_HEIGHT 24
#define MID_BOX_HEIGHT 50
#define LOW_BOX_HEIGHT 36

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

int16_t font_voffs[FONT_NBR_OF] =
{
  [FONT_STANDARD] = 5,
  [FONT_SANS_9] = 12,
  [FONT_SANS_12] = 12,
  [FONT_SANS_18] = 28,
  [FONT_SANS_24] = 40,
  [FONT_SANS_BOLD_9] = 12,
  [FONT_SANS_BOLD_12] = 12,
  [FONT_SANS_BOLD_18] = 18,
  [FONT_SANS_BOLD_24] = 24,
};
box_st box[BOX_NBR_OF];

void tftx_initialize(void)
{
  for (uint8_t i = 0; i < BOX_NBR_OF; i++)
  {
    box[i].update = true;
  }

  for (uint8_t i = BOX_MENU_1; i <= BOX_MENU_3; i++)
  {
    box[i].update     = true;
    box[i].x          = (i-1) * TFT_WIDTH/3;
    box[i].y          = TFT_HEIGHT - MENU_HEIGHT;
    box[i].w          = TFT_WIDTH/3;
    box[i].h          = MENU_HEIGHT;
    box[i].frame      = ST7735_YELLOW;
    box[i].fill       = ST7735_BLUE;
    box[i].font       = FONT_STANDARD;
    box[i].txt_color  = ST7735_CYAN;
  }
  strcpy(box[BOX_MENU_1].text, "Powr");
  strcpy(box[BOX_MENU_2].text, "Opt");
  strcpy(box[BOX_MENU_3].text, "Test");

  box[BOX_TOP].update = true;
  box[BOX_TOP].x = 0;
  box[BOX_TOP].y = 0;
  box[BOX_TOP].w = TFT_WIDTH;
  box[BOX_TOP].h = TOP_BOX_HEIGHT;
  box[BOX_TOP].frame = ST7735_WHITE;
  box[BOX_TOP].fill =  ST7735_BLACK;
  box[BOX_TOP].font = FONT_SANS_12;
  box[BOX_TOP].txt_color = ST7735_WHITE;
  strcpy(box[BOX_TOP].text, "Sending");

  box[BOX_MID].update = true;
  box[BOX_MID].x = 0;
  box[BOX_MID].y = box[BOX_TOP].y + box[BOX_TOP].h + 1;
  box[BOX_MID].w = TFT_WIDTH;
  box[BOX_MID].h = MID_BOX_HEIGHT;
  box[BOX_MID].frame = ST7735_YELLOW;
  box[BOX_MID].fill =  ST7735_BLACK;
  box[BOX_MID].font = FONT_SANS_24;
  box[BOX_MID].txt_color = ST7735_YELLOW;
  strcpy(box[BOX_MID].text, "#1234");

  box[BOX_LOW].update = true;
  box[BOX_LOW].x = 0;
  box[BOX_LOW].y = box[BOX_MID].y + box[BOX_MID].h + 1;
  box[BOX_LOW].w = TFT_WIDTH;
  box[BOX_LOW].h = LOW_BOX_HEIGHT;
  box[BOX_LOW].frame = ST7735_CYAN;
  box[BOX_LOW].fill =  ST7735_RED;
  box[BOX_LOW].font = FONT_SANS_18;
  box[BOX_LOW].txt_color = ST7735_BLACK;
  strcpy(box[BOX_LOW].text, "Err 42");



  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  Serial.println("Initialized");
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  
}

void tftx_set_font(font_et font)
{
    switch(font)
    {
      case FONT_STANDARD:
        tft.setFont();
        break;
      case FONT_SANS_9:
        tft.setFont(&FreeSans9pt7b);
        break;
      case FONT_SANS_12:
        tft.setFont(&FreeSans12pt7b);
        break;
      case FONT_SANS_18:
        tft.setFont(&FreeSans18pt7b);
        break;
      case FONT_SANS_24:
        tft.setFont(&FreeSans24pt7b);
        break;
      case FONT_SANS_BOLD_9:
        tft.setFont(&FreeSansBold9pt7b);
        break;
      case FONT_SANS_BOLD_12:
        tft.setFont(&FreeSansBold12pt7b);
        break;
      case FONT_SANS_BOLD_18:
        tft.setFont(&FreeSansBold18pt7b);
        break;
      case FONT_SANS_BOLD_24:
        tft.setFont(&FreeSansBold24pt7b);
        break;
      default:
        tft.setFont(&FreeSansBold9pt7b);
        break;
    }
}

void tftx_draw_box(box_st *boxp)
{
    tft.fillRect(boxp->x, boxp->y, boxp->w, boxp->h, boxp->fill);
    tft.drawRect(boxp->x, boxp->y, boxp->w, boxp->h, boxp->frame);
}

void tftx_update_boxes(void)
{
  for (uint8_t i = 0; i < BOX_NBR_OF; i++)
  {
      if (box[i].update)
      {
        tftx_draw_box(&box[i]);
        tft.setTextWrap(box[i].txt_wrap);
        tft.setCursor(box[i].x + 2, box[i].y + font_voffs[box[i].font]);
        //tft.setCursor(menu_box[i].x + 2, menu_box[i].y + 18);
        tftx_set_font(box[i].font);
        tft.setTextColor(box[i].txt_color);
        tft.print(box[i].text);
      }
  }
  //tftx_draw_box(&box_test);
}

#ifndef __TFTX_H__
#define __TFTX_H__

#include <Arduino.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     17
#define TFT_RST    7  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     10

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
// Option 2: use any pins but a little slower!
#define TFT_SCLK 18   // set these to be whatever pins you like!
#define TFT_MOSI 19   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

typedef enum
{
  FONT_STANDARD = 0,
  FONT_SANS_9,
  FONT_SANS_12,
  FONT_SANS_18,
  FONT_SANS_24,
  FONT_SANS_BOLD_9,
  FONT_SANS_BOLD_12,
  FONT_SANS_BOLD_18,
  FONT_SANS_BOLD_24,
  FONT_NBR_OF
} font_et;

typedef enum 
{
  BOX_FULL_SCREEN = 0,
  BOX_MENU_1,
  BOX_MENU_2,
  BOX_MENU_3,
  BOX_TOP,
  BOX_MID,
  BOX_LOW,
  BOX_NBR_OF
} box_et;

typedef struct
{
  bool    update;
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  uint16_t frame;
  uint16_t fill; 
  char text[80];
  font_et font;
  uint16_t txt_color;
  bool    txt_wrap;
} box_st;

void tftx_initialize(void);

void tftx_draw_box(box_st *boxp);

void tftx_update_boxes(void);

#endif

****************************************************************************


/**

https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/

**/



#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "tft160x128.h"
// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     17
#define TFT_RST    14  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     13

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

float p = 3.1415926;
//TFT160x128 tft160x128(&tft);

void setup(void) {
  Serial.begin(9600); 
  while(!Serial){}
  delay(2000);
  Serial.print("Hello! ST7735 TFT Test");
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
  Serial.println("Initialized");
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  Serial.print(tft.width()); Serial.print(" x "); Serial.println(tft.height());
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);

  //tft160x128.menu_box();
  //while(1);

  // large block of text
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST7735_WHITE);
  delay(1000);
  // tft print function!
  tftPrintTest();
  delay(4000);
  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST7735_GREEN);
  //delay(500);
  // line draw test
  testlines(ST7735_YELLOW);
  delay(500);
  // optimized lines
  testfastlines(ST7735_RED, ST7735_BLUE);
  delay(500);
  testdrawrects(ST7735_GREEN);
  delay(500);
  testfillrects(ST7735_YELLOW, ST7735_MAGENTA);
  delay(500);
  //tft.fillScreen(ST7735_BLACK);
  //testfillcircles(10, ST7735_BLUE);
  //testdrawcircles(10, ST7735_WHITE);
  //delay(500);
  //testroundrects();
  //delay(500);
  // testtriangles();
  //delay(500);
  //mediabuttons();
  //delay(500);
  Serial.println("done");
  delay(1000);
}

void loop() {
tft.invertDisplay(true);
delay(500);
tft.invertDisplay(false);
delay(500);
}



void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.height(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.height()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int x = 0;
  int y = 0;
  int w = tft.width()-2;
  int h = tft.height()-2;
  while ((w > 10) && (h > 10)) {
    tft.drawRoundRect(x, y, w, h, 5, color);
    x+=2;
    y+=3;
    w-=4;
    h-=6;
    color+=1100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(0, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(17, 20, 17, 60, 65, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(82, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(96, 18, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(127, 18, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(17, 20, 17, 60, 65, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(17, 20, 17, 60, 65, 40, ST77XX_GREEN);
}

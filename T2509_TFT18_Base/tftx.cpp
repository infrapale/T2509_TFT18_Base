/******************************************************************************
    tftx.cpp
    Tftx is a helper module on top of the Adafruit_GFX library. This version is 
    still a plain c-module but I am considering to convert the code into a class 
    library.
    The tftx helper divides the display into boxes. Each box has defined a dimension 
    and position. Also colors are defined for text, frame and fill. The boxes can be 
    placed on top of each other. The visibility and redraw is controlled by a 
    "updated" tag. Currently this helper does not support graphics but let's.

*******************************************************************************
    https://learn.adafruit.com/adafruit-gfx-graphics-library
    https://github.com/adafruit/Adafruit-GFX-Library
*******************************************************************************
An example how the display can be divided

  Full display      Rows              Menu                Large mid area
  ---------------   ---------------
  |             |   |             |
  |             |   ---------------                       --------------- 
  |             |   |             |                       |             |
  |             |   ---------------                       |             |
  |             |   |             |                       |             |
  |             |   ---------------   ---------------     --------------- 
  |             |   |             |   |    |    |   |
  ---------------   ---------------   ---------------


*******************************************************************************
    The application structure

    -----------------------
    |  main ino           |
    |          -----------|
    |          |  menu    |
    -----------------------
    |  dashboard          |
    -----------------------
    |  tftx               |
    -----------------------
    |  Adafruit_GFX       |
    -----------------------
    |  Adafruit_ILI9341   |
    -----------------------

 *******************************************************************************/


#include "main.h"
#include "tftx.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#if TFT_TYPE == ILI9341
  #include <Adafruit_ILI9341.h> // Hardware-specific library
#elif TFT_TYPE == ST7735
  #include <Adafruit_ST7735.h> // Hardware-specific library
#else
  #error "TFT display is not defined"
#endif
 
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
#include "main.h"
#include "io.h"

#if TFT_TYPE == ILI9341
  Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
#elif TFT_TYPE == ST7735
  Adafruit_ST7735 tft = Adafruit_ST7735(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
#endif

display_st display;
int16_t font_voffs[FONT_NBR_OF] =
{
  [FONT_STANDARD] = 5,
  [FONT_SANS_9] = 12,
  [FONT_SANS_12] = 22,
  [FONT_SANS_18] = 34,
  [FONT_SANS_24] = 40,
  [FONT_SANS_BOLD_9] = 12,
  [FONT_SANS_BOLD_12] = 22,
  [FONT_SANS_BOLD_18] = 34,
  [FONT_SANS_BOLD_24] = 40,
  [FONT_72x53RLE] = 40
};


box_st *boxp[BOX_MAX_NBR_OF];

void tftx_initialize(void)
{
  pinMode(PIN_TFT_LED, OUTPUT);  
  digitalWrite(PIN_TFT_LED, HIGH);
  analogWriteFreq(1000);
  //analogWriteRange(65535);
  //analogWriteResolution(16)

  analogWrite(PIN_TFT_LED, 64);

#if TFT_TYPE == ILI9341
  tft.begin();
#elif TFT_TYPE == ST7735
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
#endif




  #if TFT_TYPE == ILI9341
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
  #endif 

  tft.setRotation(3);
  display.width = tft.width();
  display.height = tft.height();
  display.last_box = -1;
  display.tft_type = TFT_TYPE;
  display.brightness = 4;
  Serial.printf("Screen size: %d x %d\n", display.width, display.height);
  

  for (uint8_t i = 0; i < BOX_MAX_NBR_OF; i++)
  {
    boxp[i] = NULL;
  }

  //tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  Serial.println("Initialized");
  tft.fillScreen(COLOR_BLACK);
  tft.setRotation(3);
  
}
uint16_t tftx_get_width(void)
{
  return display.width;
}

uint16_t tftx_get_height(void)
{
  return display.height;
}

uint32_t tftx_get_tft_type(void)
{
  return display.tft_type;
}

void tftx_set_brightness(uint8_t pwm_value)
{
  display.brightness = pwm_value;
}

void tftx_add_box(box_st *box_ptr)
{
  if (display.last_box < 0) display.last_box = 0; 
  if (display.last_box < BOX_MAX_NBR_OF)
  {
    boxp[display.last_box++] = box_ptr;
  }
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
      case FONT_72x53RLE:
        tft.setFont(&FreeSansBold9pt7b);
        //tft.setFont(&chrtbl_f72);
        //tft.drawBitmap( )
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
  for (uint8_t i = 0; i < BOX_MAX_NBR_OF; i++)
  {
    if (boxp[i])
    {
      if ((boxp[i]->update) && (boxp[i]->update))
      {
        boxp[i]->update = false;
        //tftx_draw_box(boxp[i]);
        tft.setTextWrap(boxp[i]->txt_wrap);
        tftx_set_font(boxp[i]->font);
        tft.setTextSize(boxp[i]->txt_size);
        tft.setCursor(boxp[i]->x + 2, boxp[i]->y + font_voffs[boxp[i]->font] * (int16_t)boxp[i]->txt_size);
        switch(1)     //(display.brightness)
        {
          case 0:
            tft.fillRect(boxp[i]->x, boxp[i]->y, boxp[i]->w, boxp[i]->h, COLOR_DARK1_FILL);
            tft.drawRect(boxp[i]->x, boxp[i]->y, boxp[i]->w, boxp[i]->h, COLOR_DARK1_FRAME);
            tft.setTextColor(COLOR_DARK1_TEXT);
            break;
          default:
            tft.fillRect(boxp[i]->x, boxp[i]->y, boxp[i]->w, boxp[i]->h, boxp[i]->fill);
            tft.drawRect(boxp[i]->x, boxp[i]->y, boxp[i]->w, boxp[i]->h, boxp[i]->frame);
            tft.setTextColor(boxp[i]->txt_color);
            break;
        }
        tft.print(boxp[i]->text);
        //Serial.println(boxp[i]->text);
      }
    } 
  }
  analogWrite(PIN_TFT_LED, display.brightness);
}

void tftx_set_text(box_st *box_ptr, char *txt_ptr)
{
    strcpy(box_ptr->text, txt_ptr);
    box_ptr->update = true;
}

void tftx_set_string(box_st *box_ptr, String *StrPtr)
{
    char txt[TXT_LEN];
    StrPtr->toCharArray(txt, TXT_LEN);
    strcpy(box_ptr->text, txt);
    box_ptr->update = true;
}


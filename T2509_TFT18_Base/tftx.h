#ifndef __TFTX_H__
#define __TFTX_H__


#define ST7735    7735
#define ILI9341   9341
#define TFT_TYPE  7735


#include <Arduino.h>


#define TXT_LEN   40

// 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
// R  R  R  R  R  G   G  G  G  G  G  B  B  B  B  B
#define COLOR_BLACK     0x0000
#define COLOR_BLUE      0x001F
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_YELLOW    0xFFE0
#define COLOR_WHITE     0xFFFF

#define COLOR_PINK      ((0x0F << 11) | (0x0F << 5) | (0x0F))
#define COLOR_GREY      ((0x03 << 11) | (0x04 << 5) | (0x04))
#define COLOR_GREY1     ((0x05 << 11) | (0x08 << 5) | (0x05))
#define COLOR_DARK_GREY ((0x02 << 11) | (0x03 << 5) | (0x03))
#define COLOR_TEAL      ((0x00 << 11) | (0x10 << 5) | (0x10))
#define COLOR_DARK_RED  ((0x13UL << 11) | (0x00UL << 5) | (0x00UL))
#define COLOR_DARK_RED2 ((0x0CUL << 11) | (0x00UL << 5) | (0x00UL))
#define COLOR_DARK_BLUE ((0x00 << 11) | (0x00 << 5) | (0x0F))

#define COLOR_DARK1_FRAME COLOR_GREY1 
#define COLOR_DARK1_FILL  COLOR_BLACK
#define COLOR_DARK1_TEXT  COLOR_GREY1 


#define BOX_MAX_NBR_OF  16

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
  FONT_72x53RLE,
  FONT_NBR_OF
} font_et;

typedef struct
{
  uint16_t  width;
  uint16_t  height;
  int8_t    last_box;
  uint32_t  tft_type;
  uint8_t   brightness;
} display_st;

typedef struct
{
  bool    update;
  bool    active;
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  uint16_t frame;
  uint16_t fill; 
  char text[80];
  font_et font;
  uint16_t txt_color;
  uint16_t txt_size;
  bool    txt_wrap;
} box_st;

void tftx_initialize(void);

void tftx_add_box(box_st *box_ptr);

void tftx_set_text(box_st *box_ptr, char *txt_ptr);

void tftx_set_string(box_st *box_ptr, String *StrPtr);

void tftx_draw_box(box_st *boxp);

void tftx_update_boxes(void);

uint16_t tftx_get_width(void);

uint16_t tftx_get_height(void);

uint32_t tftx_get_tft_type(void);

void tftx_set_brightness(uint8_t pwm_value);


#endif
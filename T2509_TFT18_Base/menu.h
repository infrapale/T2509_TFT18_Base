#ifndef __MENU_H__
#define __MENU_H__

#include <stdint.h>
#include <Arduino.h>
#define  MENU_BTN_NBR_OF  3
#define  MENU_ITEM_LEN    12
#define  MENU_LEVEL_LEN   20

typedef void (*menu_function_cb)(void);
typedef enum  
{
    MENU_MAIN = 0,
    MENU_SET_POWER,
    MENU_CLR_CNTR,
    MENU_SET_INTERVAL,
    MENU_SET_MODE,
    MENU_SETTINGS,
    MENU_RESERVED,
    MENU_NBR_OF
} menu_row_et;

typedef struct
{
    char label[MENU_ITEM_LEN];
    menu_row_et next;
    void (*cb_func)(void);
} menu_item_st;

typedef struct 
{
    char          row_label[MENU_LEVEL_LEN];
    menu_item_st menu_item[MENU_BTN_NBR_OF];
}  menu_row_st;

void menu_initialize(void);
void menu_update(void);
void menu_button_scan(void);
char menu_read_button(void);
void menu_btn_pressed(char pressed);

#endif
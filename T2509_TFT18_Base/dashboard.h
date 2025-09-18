#ifndef __DASHBOARD_H__
#define __DASHBOARD_H__
#include "tftx.h"

void dashboard_initialize(void);

void dashboard_start_task(void);

void dashboard_set_row_text(uint8_t row, char *txt);

void dashboard_set_mid_text(char *txt);

void dashboard_set_mid_color(uint16_t color);

void dashboard_set_mid_font(font_et font);

void dashboard_set_menu_label(char *label);

void dashboard_set_menu_text(uint8_t mindx, char *label);

void dashboard_next_sensor(void);

void dashboard_previous_sensor(void);

#endif
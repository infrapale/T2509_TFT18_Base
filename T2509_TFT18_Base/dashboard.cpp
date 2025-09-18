#include "main.h"
#include "tftx.h"
#include "dashboard.h"
#include "atask.h"

#define MENU_BOX_NBR_OF       3
#define ROW_BOX_NBR_OF        8

#define BOX_HEIGHT_ROW        16
#define BOX_HEIGHT_MID        140
#define BOX_HEIGHT_MENU       16

box_st menu_box[MENU_BOX_NBR_OF];
box_st mid_box;
box_st row_box[ROW_BOX_NBR_OF];

void dashboard_update_task(void);
void brightness_task(void);

//                                          123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st dashboard_task_handle        =   {"Dashboard SM   ", 1000,   0,     0,  255,    0,   1,  dashboard_update_task };
atask_st brightness_task_handle       =   {"Brigthness SM  ", 1000,   0,     0,  255,    0,   1,  brightness_task };


void dashboard_start_task(void)
{
    atask_add_new(&dashboard_task_handle);
    atask_add_new(&brightness_task_handle);
}

void dashboard_initialize(void)
{
    char txt[40];

    for (uint8_t i = 0; i < MENU_BOX_NBR_OF; i++)
    {
        menu_box[i].update     = true;
        menu_box[i].active     = true;
        menu_box[i].x          = i * tftx_get_width() / MENU_BOX_NBR_OF;
        menu_box[i].y          = tftx_get_height() - BOX_HEIGHT_MENU;
        menu_box[i].w          = tftx_get_width()/MENU_BOX_NBR_OF;
        menu_box[i].h          = BOX_HEIGHT_MENU;
        menu_box[i].frame      = COLOR_GREY;
        menu_box[i].fill       = COLOR_TEAL;
        menu_box[i].font       = FONT_STANDARD;  FONT_SANS_12;
        menu_box[i].txt_color  = COLOR_WHITE;
        menu_box[i].txt_size   = 1;
        tftx_add_box(&menu_box[i]);
    }
    strcpy(menu_box[0].text, "Menu0");
    strcpy(menu_box[1].text, "Menu1");
    strcpy(menu_box[2].text, "Menu2");
  
    for (uint8_t i = 0; i < ROW_BOX_NBR_OF; i++)
    {
        row_box[i].update     = true;
        row_box[i].active     = true;
        row_box[i].x          = 0;
        row_box[i].y          = i*BOX_HEIGHT_ROW ;
        row_box[i].w          = tftx_get_width();
        row_box[i].h          = BOX_HEIGHT_ROW ;
        row_box[i].frame      = COLOR_GREY;
        row_box[i].fill       = COLOR_BLUE;
        row_box[i].font       = FONT_STANDARD;   //FONT_SANS_12;
        row_box[i].txt_color  = COLOR_WHITE;
        row_box[i].txt_size   = 1;
        sprintf(txt,"Row %d",i);
        strcpy(row_box[i].text, txt);
        tftx_add_box(&row_box[i]);
    }
    // blocking mid large area
    for (uint8_t i = 2; i < 6; i++)
    {
        row_box[i].active = false;
        row_box[i].update = false;
    }
    // blocking menu row    
    row_box[ROW_BOX_NBR_OF -1].active = false;
    row_box[ROW_BOX_NBR_OF -1].update = false;
    row_box[0].frame      = COLOR_GREY;
    row_box[0].fill       = COLOR_DARK_RED2;
    row_box[0].txt_color  = COLOR_WHITE;
    row_box[1].fill       = COLOR_DARK_RED;
    row_box[6].fill       = COLOR_DARK_RED;
    sprintf(txt, "%s", APP_NAME);
    tftx_set_text(&row_box[0], txt);


    mid_box.active = false;
    mid_box.update = true;
    mid_box.x = 0;
    mid_box.y = row_box[3].y;
    mid_box.w = tftx_get_width();
    mid_box.h = row_box[0].h * 3;
    mid_box.frame = COLOR_YELLOW;
    mid_box.fill =  COLOR_DARK_GREY;
    mid_box.font = FONT_SANS_24;
    mid_box.txt_color = COLOR_YELLOW;
    mid_box.txt_size = 1;
    strcpy(mid_box.text, " 12:34");
    tftx_add_box(&mid_box); 
}

void dashboard_set_row_text(uint8_t row, char *txt)
{
    tftx_set_text(&row_box[row], txt);
}

void dashboard_set_mid_text(char *txt)
{
    tftx_set_text(&mid_box, txt);
}
void dashboard_set_mid_color(uint16_t color)
{
   mid_box.txt_color = color;
}
void dashboard_set_mid_font(font_et font)
{
    mid_box.font = font;
}

void dashboard_set_menu_label(char *label)
{
    tftx_set_text(&row_box[6], label);
}

void dashboard_set_menu_text(uint8_t mindx, char *label)
{
    tftx_set_text(&menu_box[mindx], label);
}


void dashboard_show_info(void)
{
    //char txt[40];
    //sprintf(txt, "%s", APP_NAME);
    //tftx_set_text(&row_box[0], txt);
    //sprintf(txt, "%s %s", __DATE__, __TIME__);
    //tftx_set_text(&row_box[1], txt); 
}


void dashboard_show_common(void)
{
    String Str;
    {
        Str = MAIN_TITLE;
    }
}


void dashboard_update_task(void)
{
    static uint32_t next_step_ms;
    bool            update_box;
    String          Str;
    uint8_t         i; 
    char            txt[20];
    
    switch (dashboard_task_handle.state)
    {
        case 0:
            dashboard_show_info();
            dashboard_task_handle.state++;
            break;
        case 1:                
            dashboard_show_common();
            dashboard_task_handle.state++;
            break;
        case 2:
            update_box = false;
            break;  
        case 3:
              
            break;
    }
    //Serial.printf("db %d -> %d\n", dashboard_task_handle.prev_state, dashboard_task_handle.state);
    tftx_update_boxes();
}

void brightness_task(void)
{
    static uint32_t timeout_ms;
    uint16_t ldr_value;

    ldr_value = io_read_ldr();
    uint16_t pwm = map(ldr_value,0,3500,2,64);
    if (pwm > 64) pwm = 64;
    pwm = (pwm*pwm)/16;
    if (pwm > 255) pwm = 255;
    if (pwm == 0) pwm = 4;
    //Serial.printf("PWM=%d\n",pwm);
    pwm = 128;
    switch(brightness_task_handle.state)
    {
        case 0:
            brightness_task_handle.state = 10;
            tftx_set_brightness(8);
            break;
        case 10:
            if (io_read_pir())
            {
                timeout_ms = millis() + 30000;
                tftx_set_brightness((uint8_t) pwm);
                //tftx_set_brightness(255);
                brightness_task_handle.state = 100;
            }
        case 100:
            if ( millis() > timeout_ms)
            {
                if (!io_read_pir())
                {
                    tftx_set_brightness(4);
                    brightness_task_handle.state = 10;
                }
                else tftx_set_brightness((uint8_t) pwm);
            }
    }
}

void dashboard_next_sensor(void)
{
/*    dashboard_ctrl.menu_sensor_indx++;
    if(dashboard_ctrl.menu_sensor_indx >= AIO_SUBS_NBR_OF) dashboard_ctrl.menu_sensor_indx = AIO_SUBS_TRE_ID_TEMP;
    subs_data[dashboard_ctrl.menu_sensor_indx].show_next_ms = 0              ;
    dashboard_ctrl.sensor_indx = dashboard_ctrl.menu_sensor_indx;
    Serial.printf("dashboard_ctrl.menu_sensor_indx=%d\n",dashboard_ctrl.menu_sensor_indx);
    dashboard_ctrl.fast_forward = true;
*/
}

void dashboard_previous_sensor(void)
{
/*    if(dashboard_ctrl.menu_sensor_indx <= 1 ) dashboard_ctrl.menu_sensor_indx = AIO_SUBS_NBR_OF -1;
    else dashboard_ctrl.menu_sensor_indx--;
    subs_data[dashboard_ctrl.menu_sensor_indx].show_next_ms = 0;
    dashboard_ctrl.sensor_indx = dashboard_ctrl.menu_sensor_indx;
    Serial.printf("dashboard_ctrl.menu_sensor_indx=%d\n",dashboard_ctrl.menu_sensor_indx);
    dashboard_ctrl.fast_forward = true;
*/    
}




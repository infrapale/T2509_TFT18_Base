
#include "main.h"
#include "menu.h"
#include "atask.h"
#include "BtnPinOnOff.h"
#include "dashboard.h"
#include "tftx.h"
#include "lora.h"

#define NBR_MENU_KEYS  4
#define NBR_RADIO_PWR  6
#define NBR_INTERVAL   5


extern lora_st lora;
void dummy_cb()
{
   // 
}

typedef enum
{
  MODE_REMOTE = 0,
  MODE_BASE,
  MODE_RELAY,
  MODE_NBR_OF
} mode_et;

typedef struct
{
    uint8_t atask;
    uint8_t active;
    uint8_t mode_indx;
    uint8_t pwr_indx;
    uint8_t ival_indx;
    uint8_t modem_conf_indx;
} menu_ctrl_st;


void menu_test1(void)
{
   Serial.printf("Test 1\n");
}

menu_ctrl_st menu_ctrl = {ATASK_NOT_DEFINED,  MENU_MAIN, MODE_REMOTE, 0};

uint8_t radio_pwr[NBR_RADIO_PWR] = {2,4,8,10,14,15};
uint8_t send_ival[NBR_INTERVAL]  = {5,10,20,30,60};
char    mode_txt[MODE_NBR_OF][8] = {"Remote","Base","Relay"};

char    modem_conf_label[NBR_OF_MODEM_CONF][17] =
{ // 1234567890123456
	  "Bw125Cr45Sf128  ",    ///< Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
	  "Bw500Cr45Sf128  ",    ///< Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
	  "Bw31_25Cr48Sf512",	   ///< Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range
	  "Bw125Cr48Sf4096 ",    ///< Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, low data rate, CRC on. Slow+long range
	  "Bw125Cr45Sf2048 "     ///< Bw = 125 kHz, Cr = 4/5, Sf = 2048chips/symbol, CRC on. Slow+long range 
};


void menu_next_mode(void)
{
  if(++menu_ctrl.mode_indx >= MODE_NBR_OF  )  menu_ctrl.mode_indx = MODE_REMOTE;
}

void menu_next_pwr(void)
{
  if(++menu_ctrl.pwr_indx >= NBR_RADIO_PWR )  menu_ctrl.pwr_indx = 0;
  lora.power = radio_pwr[menu_ctrl.pwr_indx];
  lora_set_power(lora.power);
}

void menu_next_interval(void)
{
  if(++menu_ctrl.ival_indx >= NBR_INTERVAL )  menu_ctrl.ival_indx = 0;
  lora.interval = send_ival[menu_ctrl.ival_indx];
}

void menu_next_modem_conf(void)
{
  if(++menu_ctrl.modem_conf_indx >= NBR_OF_MODEM_CONF )  menu_ctrl.modem_conf_indx = 0;
  lora_set_modem_conf( menu_ctrl.modem_conf_indx);
}


void menu_pwr_down(void)
{
  if(menu_ctrl.pwr_indx >= 0 )  menu_ctrl.pwr_indx++;
  lora.power = radio_pwr[menu_ctrl.pwr_indx];
}


void menu_clr_cntr(void)
{
  lora.my_counter = 0;
}

menu_row_st menu[MENU_NBR_OF] =
{
  [MENU_MAIN] =
  {
    "Main Menu", 
    {
      {"Option  ", MENU_SETTINGS, dummy_cb},
      {"Power   ", MENU_MAIN, menu_next_pwr },
      {"Conf    ", MENU_MAIN, menu_next_modem_conf},
    }
  },
  [MENU_SET_POWER] =
  {
    "Set Power", 
    {
      {"Function",  MENU_CLR_CNTR, dummy_cb },
      {"Back    ",  MENU_MAIN, dummy_cb},
      {"Power++",   MENU_SET_POWER, menu_next_pwr},
    }
  },
  [MENU_CLR_CNTR] =
  {
    "Clear Counter", 
    {
      {"Function",  MENU_SET_INTERVAL, dummy_cb },
      {"Back    ",  MENU_MAIN, dummy_cb},
      {"Clear   ",  MENU_CLR_CNTR, menu_clr_cntr},
    }
  },

  [MENU_SET_INTERVAL] =
  {
    "Set Interval", 
    {
      {"Function",  MENU_MAIN, dummy_cb },
      {"Back    ",  MENU_MAIN, dummy_cb},
      {"Ival++  ",  MENU_SET_INTERVAL, menu_next_interval},
    }
  },
    
  [MENU_SET_MODE] =
  {
    "Set Mode", 
    {
      {"Function",  MENU_MAIN, dummy_cb },
      {"Back    ",      MENU_MAIN, dummy_cb},
      {"Mode    ",      MENU_MAIN, dummy_cb},
    }
  },
  [MENU_SETTINGS] =
  {
    "Settings", 
    {
      {"Main",  MENU_MAIN, dummy_cb },
      {"Prev.", MENU_SETTINGS, dummy_cb},
      {"Next",  MENU_SETTINGS, dummy_cb},
     }
  },
  [MENU_RESERVED] =
  {
    "Outdoor Sensors", 
    {
      {"Main",  MENU_MAIN, dummy_cb },
      {"YYYY",  MENU_RESERVED, dummy_cb},
      {"ZZZZ",  MENU_RESERVED, dummy_cb},
    }
  }
};


BtnPinOnOff  menu_btn[NBR_MENU_KEYS];

void menu_read_machine(void);

//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st menu_key_task_handle =   {"Menu Key Task  ", 100,    0,     0,  255,    0,   1, menu_read_machine };
atask_st menu_key_scan_handle =   {"Menu Key Scan  ", 10,     0,     0,  255,    0,   1, menu_button_scan };


void menu_initialize(void)
{
  menu_btn[0].Init(PIN_KEY1,'0', true);
  menu_btn[1].Init(PIN_KEY2,'1', true);
  menu_btn[2].Init(PIN_KEY3,'2', true);
  
  atask_add_new(&menu_key_task_handle);
  atask_add_new(&menu_key_scan_handle);
  menu_update();

}


void menu_read_machine(void)
{
  char c = menu_read_button();
  if (c != 0x00) 
  {
      if ((c & 0b10000000) == 0) 
          Serial.printf("On");
      else 
          Serial.printf("Off");
      Serial.printf(" %c\n",c & 0b01111111);
      c &=  0b01111111;
      menu_btn_pressed(c);
  }

}

void menu_update(void)
{
    char txt[32];
    dashboard_set_menu_label(menu[menu_ctrl.active].row_label);

    for (uint8_t i = 0; i < MENU_BTN_NBR_OF; i++)
    {
        dashboard_set_menu_text(i, menu[menu_ctrl.active].menu_item[i].label);
    }

    sprintf(txt,"%s Pwr %ddB",
        modem_conf_label[lora.modem_conf_indx],
        radio_pwr[menu_ctrl.pwr_indx]
    );
    dashboard_set_row_text(1, txt);
    
    sprintf(txt,"%s - %s",
        APP_NAME,
        mode_txt[lora.role]
    );
    dashboard_set_row_text(0, txt);



}

void menu_button_scan(void)
{
    for( uint8_t i= 0; i < NBR_MENU_KEYS; i++)
    {
        menu_btn[i].Scan();
    }
}

char menu_read_button(void)
{
    char c = 0x00;
    for( uint8_t i= 0; i < NBR_MENU_KEYS; i++)
    {
        c = menu_btn[i].Read();
        if (c != 0x00) break;
    }
    return c;
}

void menu_btn_pressed(char pressed)
{
    uint8_t bindx = pressed - '0';
    if (bindx < MENU_BTN_NBR_OF)
    {
        Serial.printf("active %d - pressed %d\n", menu_ctrl.active, bindx);
        menu[menu_ctrl.active].menu_item[bindx].cb_func();
        menu_ctrl.active = menu[menu_ctrl.active].menu_item[bindx].next;
        menu_update();
    }
}

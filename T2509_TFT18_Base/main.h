#ifndef __MAIN_H__

#define __MAIN_H__


#define BOARD_PICO_TFT_4KEYS
#define PIRPANA
//#define LILLA_ASTRID
//#define VILLA_ASTRID

#include "io.h"
#include "atask.h"

#define LABEL_LEN           12
#define TIME_ZONE_OFFS      3
#define UNIT_LABEL_LEN      10
#define MEASURE_LABEL_LEN   16

#define APP_NAME   "T2509_TFT18_Base"
#define MAIN_TITLE "Villa Astrid Console"

typedef enum
{
    NODE_ROLE_UNDEFINED = 0,
    NODE_ROLE_CLIENT,
    NODE_ROLE_SERVER,
    NODE_ROLE_NBR_OF
} node_role_et;


typedef struct date_time
{
    uint16_t  year;
    uint8_t   month;
    uint8_t   day;
    uint8_t   hour;
    uint8_t   minute;
    uint8_t   second;
 } date_time_st;

typedef struct 
{
    node_role_et    role;
    uint8_t         power;
    uint8_t         my_addr;
    uint8_t         base_addr;
    uint16_t        frequency;
    uint8_t         spreading_factor;
    uint16_t        my_counter;
    uint16_t        base_counter;
    uint16_t        interval;
    uint16_t        ival_cntr;
    int             my_rssi;
    uint8_t         modem_conf_indx;
} lora_st;

#endif
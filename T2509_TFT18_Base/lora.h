#ifndef __LORA_H__
#define __LORA_H__


#define CMD_TAG_LEN         5
#define CMD_MAX_VALUES      10
#define CMD_FIELD_LEN       8

#define NBR_OF_MODEM_CONF   5

typedef enum
{
  FIELD_START   = 0,
  FIELD_COMMAND,
  FIELD_FROM,
  FIELD_TARGET,
  FIELD_RADIO,
  FIELD_POWER,
  FIELD_SPREADING_FACTOR,
  FIELD_REMOTE_NBR,
  FIELD_BASE_NBR,
  FILED_END,
  FIELD_NBR_OF
} field_et;

typedef enum
{
  CMD_UNDEFINED = 0,
  CMD_RADIO_SEND,
  CMD_RADIO_RECEIVE, 
  CMD_SET_POWER,
  CMD_RADIO_RESET,
  CMD_SET_SF,
  CMD_RADIO_REPLY,
  CMD_GET_RSSI,
  CMD_GET_ROLE,
  CMD_GET_MSG,
  CMD_GET_CNTR,
  CMD_NBR_OF
} cmd_et;

typedef enum
{
  RADIO_UNDEFINED = 0,
  RADIO_LORA_433  = 1,
  RADIO_LORA_868  = 2,
  RADIO_RFM69_433 = 3,
  RADIO_RFM69_868 = 4,
} radio_et;

typedef enum
{
  RX_MSG_SEND_REMOTE_TO_BASE,
  RX_MSG_RESET_CNTR,
  RX_MSG_,
  RX_MSG_NBR_OF
} rx_msg_et;

typedef enum
{
  TX_MSG_SEND_REMOTE_TO_BASE,
  TX_MSG_RESET_CNTR,
  TX_MSG_,
  TX_MSG_NBR_OF
} tx_msg_et;

typedef enum
{
  STATUS_UNDEFINED = 0,
  STATUS_AVAILABLE,
  STATUS_OK_FOR_ME,
  STATUS_INCORRECT_FRAME,
} msg_status_et;


typedef struct
{
  char      start;
  char      cmnd[CMD_TAG_LEN];
  uint8_t   from;
  uint8_t   target;
  char      field[CMD_MAX_VALUES][CMD_FIELD_LEN];
  uint8_t   radio;
  uint8_t   power;
  int16_t   rssi;
  uint8_t   sf;
  uint16_t  remote_nbr;
  uint16_t  base_nbr;
  char      end;
} field_st;


typedef struct
{
    char  tag[CMD_TAG_LEN];
} cmd_st;


typedef struct
{
  char      tag[CMD_TAG_LEN];
  cmd_et    tag_indx;
  uint8_t   nbr_values;
  int16_t   value[CMD_MAX_VALUES];
  char      field[CMD_MAX_VALUES][CMD_FIELD_LEN];
  uint8_t   sender;
  uint8_t   target;
  float     temperature;
  uint16_t  counter;

} msg_data_st;

typedef struct
{
  String          str;
  uint8_t         len;
  field_st        field;
  msg_status_et   status;
  //uint8_t         rx_delim[FIELD_NBR_OF];
  bool            avail;
} msg_st;


void lora_initialize(void);

void lora_set_power(uint8_t pwr);

void lora_set_modem_conf(uint8_t modem_conf_indx);


#endif
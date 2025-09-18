/*******************************************************************************
https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/
********************************************************************************
Required non-arduino Libraries:
  https://github.com/infrapale/Pin_Button
  https://github.com/infrapale/T2409_atask
********************************************************************************
LoRa MCU Code:
  https://github.com/infrapale/T2505_RFM95_Test   .._Test4  !!!
  
*******************************************************************************/

#include <SPI.h>
#include "main.h"
#include "tftx.h"
#include "atask.h"
#include "dashboard.h"
#include "logger.h"
#include "menu.h"
#include "lora.h"

lora_st lora = 
{
    .role             = NODE_ROLE_UNDEFINED,
    .power            = 14,
    .my_addr          = 1,
    .base_addr        = 2,
    .frequency        = 868,
    .spreading_factor = 12,
    .my_counter       = 0,
    .base_counter     = 0,
    .interval         = 10,
    .ival_cntr        = 0,
    .my_rssi          = 0
};
void print_debug_task(void);

//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st debug_task_handle    =   {"Debug Task     ", 5000,    0,     0,  255,    0,  1,  print_debug_task };


void setup(void) 
{
  Serial1.setTX(PIN_UART0_TX);   // UART0
  Serial1.setRX(PIN_UART0_RX);
  Serial2.setTX(PIN_UART1_TX);   // UART1
  Serial2.setRX(PIN_UART1_RX);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  //while(!Serial){}
  delay(2000);
  Serial.print(APP_NAME); Serial.print(" Compiled: ");
  Serial.print(__DATE__); Serial.print(" ");
  Serial.print(__TIME__); Serial.println();
  io_initialize();
  atask_initialize();
  atask_add_new(&debug_task_handle);
  logger_initialize(); 
  tftx_initialize();
  dashboard_initialize();
  dashboard_start_task();
  menu_initialize();
  lora_initialize();
  tftx_update_boxes();
}

void loop() {
  atask_run();
  /*
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);
  */
}

void print_debug_task(void)
{
  atask_print_status(true);
  //Serial.printf("LDR= %d  PIR = %d\n", io_read_ldr(), io_read_pir());
  //Serial1.println("Print to UART0 TX ");
  // if (Serial1.available())
  // {
  //     String  rx_str;
  //     rx_str = Serial1.readStringUntil('\n');
  //     Serial.println(rx_str);
  // }
}




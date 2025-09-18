#include "main.h"
#include "atask.h"
#include "menu.h"
#include "lora.h"
#include "tftx.h"
#include "dashboard.h"


extern lora_st lora;

void lora_task(void);

//                                          123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st lora_th                      =   {"LoRa Task      ", 100,   0,     0,  255,    0,   1,  lora_task };

cmd_st commands[CMD_NBR_OF] = 
{
  [CMD_UNDEFINED]       = {"UNDF"},
  [CMD_RADIO_SEND]      = {"RSND"},
  [CMD_RADIO_RECEIVE]   = {"RREC"},
  [CMD_SET_POWER]       = {"SPWR"},
  [CMD_RADIO_RESET]     = {"RRST"},
  [CMD_SET_SF]          = {"S_SF"},
  [CMD_RADIO_REPLY]     = {"RREP"},
  [CMD_GET_RSSI]        = {"RSSI"},
  [CMD_GET_ROLE]        = {"ROLE"},
  [CMD_GET_MSG]         = {"GMSG"},
  [CMD_GET_CNTR]        = {"CNTR"},
};

char node_role[NODE_ROLE_NBR_OF][8] =
{
   [NODE_ROLE_UNDEFINED]   = "Undef  ",
   [NODE_ROLE_CLIENT]      = "Client ",
   [NODE_ROLE_SERVER]      = "Server "
};



msg_st      rply_msg;
msg_data_st msg_values  = {0};


void lora_initialize(void)
{
    atask_add_new(&lora_th);
}

void lora_send_msg(void)
{
   char buff[40];
   memset(buff,0x00,40);
   sprintf(buff,"<RSND;%d;%d;%d;%d;%d;%d;%d;%d>",
      lora.my_addr,
      lora.base_addr,
      1,
      lora.power,
      0,
      lora.spreading_factor,
      lora.my_counter,
      lora.base_counter);
   Serial.println(buff);   
   Serial1.println(buff);   
   //Serial1.println("ABC");
}


void parser_rd_msg_fields(msg_data_st *msg_data, String *StrP)
{
    int     indx1 = StrP->indexOf('<');
    int     indx2 = StrP->indexOf('>');
    bool    do_continue = true;
    int     end_pos = indx2;
    String  SubStr;

    if ((indx1 >= 0)&&(indx2>indx1)) do_continue = true;
    else do_continue = false;
    if (do_continue) {

        SubStr = StrP->substring(indx1,indx2);
        //Serial.print("SubStr="); Serial.println(SubStr);
        end_pos = SubStr.length();
        msg_data->nbr_values = 0;
        if((indx1 < end_pos)) do_continue = true;
        else do_continue = false;
    }
    //memset(msg_data->field,0x00, sizeof(msg_data->field));
    for (uint8_t i=0; i < CMD_MAX_VALUES; i++) msg_data->field[i][0] = 0x00;
    indx1++;
    while(do_continue)
    {
        indx2 = SubStr.indexOf(';',indx1);
        if (indx2 < 0) {
            indx2 = end_pos;
            do_continue = false;
        }
        //msg_data->value[msg_data->nbr_values] = StrP->substring(indx1,indx2).toInt();
        SubStr.substring(indx1,indx2).toCharArray(msg_data->field[msg_data->nbr_values],CMD_FIELD_LEN);
        indx1 = indx2+1;
        msg_data->nbr_values++;
        if ((indx2 >= end_pos) || (msg_data->nbr_values >= CMD_MAX_VALUES)) do_continue = false;
    }
}

void parser_set_data(msg_data_st *msg_data)
{
    uint8_t findx = 0;
    bool    do_continue = true;
    String  Str;
    char    id;
    if (msg_data->nbr_values < 4) do_continue = false; 
    if (do_continue){
        Str = msg_data->field[0];
        msg_data->target = Str.toInt();
        Str = msg_data->field[1];
        msg_data->sender = Str.toInt();
        for (uint8_t i = 2; i < msg_data->nbr_values; i++ )
        {
            id = msg_data->field[i][0];
            Str = &msg_data->field[i][1];
                switch(id)
            {
                case 'T':
                    msg_data->temperature = Str.toFloat();
                    break;
                case 'C':
                    msg_data->counter = Str.toInt();
                    break;    
            }
        }
    }
}

void parser_print_data(msg_data_st *msg_data)
{ 
    Serial.printf("len = %d: ",  msg_data->nbr_values);
    for (uint8_t i = 0; i < CMD_MAX_VALUES; i++)
    {
        Serial.printf("%s, ",msg_data->field[i]);
    }
    Serial.println("");

    Serial.printf("Target=%d Sender=%d Temperature=%f.1, Counter=%d\n",
        msg_data->target,
        msg_data->sender,
        msg_data->temperature,
        msg_data->counter
    );
}

void parser_build_msg_from_fields(char *msg, msg_st *msg_data)
{
    sprintf(msg,"<%s;%d;%d;%d;%d;%d;%d;%d;%d>",
        msg_data->field.cmnd,
        msg_data->field.from,
        msg_data->field.target,
        msg_data->field.radio,
        msg_data->field.power,
        msg_data->field.rssi,
        msg_data->field.sf,
        msg_data->field.remote_nbr,
        msg_data->field.base_nbr
    );
}

msg_status_et lora_check_frame(String *Str)
{
   msg_status_et status = STATUS_UNDEFINED;
   //rfm_send_msg_st *rx_msg = &send_msg; 
   bool do_continue = true;
   Str->trim();
   uint8_t len = Str->length();
   // Serial.print(*Str); Serial.println(len);
   if ((Str->charAt(0) != '<') || 
        (Str->charAt(len-1) != '>'))  do_continue = false;
   if (do_continue)
   {   
        status = STATUS_OK_FOR_ME;
        #ifdef DEBUG_PRINT
        Serial.print("Buffer frame is OK\n");
        #endif
   }
   else status = STATUS_INCORRECT_FRAME;
   return status;
}



bool lora_receive_msg(void)
{
      String Str;
      bool msg_received = false;
      msg_status_et rply_status = STATUS_UNDEFINED;
      

      if (Serial1.available())
      {
            Serial.print("Message: ");
            Str = Serial1.readStringUntil('\r');
            Serial.println(Str);
            if (Str.length()> 0)
            {
               parser_rd_msg_fields(&msg_values, &Str);
               parser_set_data(&msg_values);
               parser_print_data(&msg_values);


               rply_status = lora_check_frame(&Str);
               if (rply_status == STATUS_OK_FOR_ME) 
               {
                  parser_rd_msg_fields(&msg_values, &Str);
                  // rply_msg.field.from        = msg_values.value[0];
                  // rply_msg.field.target      = msg_values.value[1];
                  // rply_msg.field.radio       = msg_values.value[2];
                  // rply_msg.field.power       = msg_values.value[3];
                  // rply_msg.field.rssi        = msg_values.value[4];
                  // rply_msg.field.sf          = msg_values.value[5];
                  // rply_msg.field.remote_nbr  = msg_values.value[6];
                  // rply_msg.field.base_nbr    = msg_values.value[7];
                  msg_received = true;
               }
               else 
               {
                  Serial.println("lora_receive_msg: Message was not OK");
               }
                
            }
      } 
      return msg_received;
}

void lora_set_power(uint8_t pwr)
{
   Serial1.printf("<SPWR;%d>\n",pwr);   
}

void lora_set_modem_conf(uint8_t modem_conf_indx)
{
   lora.modem_conf_indx =  modem_conf_indx;
   Serial1.printf("<SMCF;%d>\n",modem_conf_indx);   
}


void lora_request_rssi(void)
{
   Serial1.println("<RSSI;0>");   
}

int lora_get_rssi(void)
{
      String Str;
      int rssi = 0;

      if (Serial1.available())
      {
            Str = Serial1.readStringUntil('\n');
            if (Str.length()> 0)
            {
                  msg_status_et rply_status = lora_check_frame(&Str);
                  if (rply_status == STATUS_OK_FOR_ME) 
                  {
                     parser_rd_msg_fields(&msg_values, &Str);
                     //rssi         = msg_values.value[0];
                     Serial.println(Str);
                  }
            }
      } 
      return rssi;
}

bool lora_get_param(int16_t *param, cmd_et cmd)
{
      String   Str;
      uint32_t timeout;
      bool     got_param = false;
      char     txt[12];

      sprintf(txt,"<%s;0>\n", commands[cmd].tag);   
      Serial1.print(txt);
      Serial.print(txt);  
      timeout = millis() + 1000;
      while(!Serial1.available() && (millis() < timeout)) {}


      if (Serial1.available())
      {
            Str = Serial1.readStringUntil('\n');
            Serial.println(Str);
            if (Str.length()> 0)
            {
                  msg_status_et rply_status = lora_check_frame(&Str);
                  if (rply_status == STATUS_OK_FOR_ME) 
                  {
                     got_param = true;
                     parser_rd_msg_fields(&msg_values, &Str);
                     parser_set_data(&msg_values);
                     parser_print_data(&msg_values);

                     // *param = msg_values.value[0];
                  }
            }
      } 
      return got_param;
}


void lora_request_param(cmd_et cmd)
{
   Serial1.printf("<%s;0>\n", commands[cmd]);   
}

void lora_request_msg(void)
{
   Serial1.println("<GMSG;0>");   
}


void lora_update_client_dashboard(void)
{
   char txt[40];

   sprintf(txt,"Remote# %d Base# %d",
   rply_msg.field.remote_nbr,
   rply_msg.field.base_nbr);
   dashboard_set_row_text(2, txt);

   //sprintf(txt,"R:%d B:%d",  lora.my_rssi, rply_msg.field.rssi );
   sprintf(txt,"%f0.1C", msg_values.temperature );
   dashboard_set_mid_text(txt);
   if(lora.my_rssi < 0)
   {
      dashboard_set_mid_color(COLOR_YELLOW);
      dashboard_set_mid_font(FONT_SANS_18);
   }   
   else
   {
      dashboard_set_mid_text((char*)"Hetkinen");
      dashboard_set_mid_color(COLOR_RED);
      dashboard_set_mid_font(FONT_SANS_18);
   } 
   menu_update();

}

void lora_update_server_dashboard(void)
{
   char txt[40];
   sprintf(txt,"Remote# %d Base# %d",
      rply_msg.field.remote_nbr,
      lora.my_counter);
   dashboard_set_row_text(2, txt);

   sprintf(txt,"%f0.1 C", msg_values.temperature );
   dashboard_set_mid_text(txt);
   if(true)   //(lora.my_rssi < 0)
   {
      dashboard_set_mid_color(COLOR_YELLOW);
      dashboard_set_mid_font(FONT_SANS_24);
   }   
   else
   {
      dashboard_set_mid_text((char*)"Hetkinen");
      dashboard_set_mid_color(COLOR_RED);
      dashboard_set_mid_font(FONT_SANS_18);
   } 
   menu_update();

}




void lora_task(void)
{
   static uint32_t timeout_ms;
   static int16_t nrole = 0;
   static int16_t i16;

   switch(lora_th.state)
   {
      // ***************** Common Code ***************************    
      case 0:
         lora_th.state = 5;
         timeout_ms = millis() + 1000;
         //io_ext_clr(true);  //Reset LoRa MCU
         break;
      case 5:
         if (millis() > timeout_ms)
         {
            lora_th.state = 6;
            io_ext_clr(false);  
            timeout_ms = millis() + 4000;
         }
         break;
      case 6:
         if (millis() > timeout_ms)
         {
            lora_th.state = 10;
         }
      case 10:
         // if(lora.role == NODE_ROLE_UNDEFINED)
         // {
         //    lora_get_param(&nrole, CMD_GET_ROLE);
         //    lora.role = (node_role_et) nrole;           
         //    Serial.print("Node role = "); Serial.println(node_role[lora.role]);
         // }
         timeout_ms = millis() + 50;
         lora_th.state = 11;
         break;
      case 11:
         if (millis() > timeout_ms) 
         {
            lora_th.state = 200;    
         }
         break;
      // ***************** Client Code ***************************    
      case 20:
         lora.my_counter++;
         Serial.printf("Cntr = %d\n", lora.my_counter);
         lora_send_msg();
         lora_th.state = 30;
         timeout_ms = millis() + 2000;
         break;
      case 30:
         if (lora_receive_msg())
         {
            lora_th.state = 50;
         }
         else
         {
            if (millis() > timeout_ms) lora_th.state = 40;
         }
         break;
      case 40:
         Serial.println("No reply received");
         lora.my_rssi = 0;
         lora_th.state = 100;
         break;
      case 50:
         lora_request_rssi();
         timeout_ms = millis() + 500;
         lora_th.state = 51;
         break;
      case 51:
         lora.my_rssi = lora_get_rssi();
         //Serial.printf("lora.my_rssi=%d\n",lora.my_rssi);
         if((lora.my_rssi < 0) || (millis() > timeout_ms)) lora_th.state = 100;
         break;
      case 100:
         lora_update_client_dashboard();
         lora_th.state = 10;
         break;

      case 200:
         if (lora_receive_msg())
         {
            lora_update_server_dashboard();
            lora_th.state = 10;
         }
         else
         {
            if (millis() > timeout_ms) lora_th.state = 10;
         }
         break;
   
      // ***************** Server Code ***************************    
      case 500:  
         timeout_ms = millis() + 10000;
         lora_th.state = 505;
         break; 
      case 505:
         lora_request_msg();
         lora_th.state = 510;
         break;
      case 510:  
         if (lora_receive_msg()) 
         {
            lora_th.state = 530;
         }   
         else 
         {
            if (millis() > timeout_ms) lora_th.state = 500;
         }
         break;
      case 520:  
         Serial.println("No request received");
         lora_th.state = 10;
         break; 
      case 530:  
         //Serial.printf("rply_msg.field.remote_nbr=%d\n",rply_msg.field.remote_nbr);
         lora_th.state = 540;
         lora_request_rssi();
         timeout_ms = millis() + 500;
         break; 
      case 540:  
         lora.my_rssi = lora_get_rssi();
         //Serial.printf("lora.my_rssi=%d\n",lora.my_rssi);
         if((lora.my_rssi < 0) || (millis() > timeout_ms)) lora_th.state = 550;
         break; 
      case 550:  
         lora_get_param(&i16, CMD_GET_CNTR);
         lora.my_counter = (uint16_t) i16;
         lora_th.state = 600;
         break; 
      case 600:  
         lora_update_server_dashboard();
         lora_th.state = 10;
         break; 
      
   }
   
}

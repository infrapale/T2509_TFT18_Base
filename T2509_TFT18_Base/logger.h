#ifndef __LOGGER_H__
#define __LOGGER_H__

#define LOGGER_DATA_LEN  80

typedef struct
{
    char msg_buff[LOGGER_DATA_LEN];
    char file_name[32];
    bool sd_ok;
    bool do_log;
    uint8_t task_indx;
} logger_st;


void logger_initialize(void);
void logger_do_log(void);
void logger_add_msg(char *new_msg);
#endif
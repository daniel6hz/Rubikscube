/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004-2006
Part of BlueLab 3.5.2-release

FILE NAME
    spp_dev_private.h
    
DESCRIPTION
    
*/

#ifndef _SPP_DEV_PRIVATE_H_
#define _SPP_DEV_PRIVATE_H_

#include <spp.h>
#include <message.h>
#include <app/message/system_message.h>


#include "spp_button.h"

#define LED             /*1<<0*/0

#define PIO_CONNECTED   /*1<<1*/0

#define SPP_MSG_BASE    (0x100)


enum
{
VALUE_PIO0_PRESS    = 0x01,
VALUE_PIO1_PRESS    = 0x11,
VALUE_PIO2_PRESS    = 0x21,
VALUE_PIO3_PRESS	= 0x31,
VALUE_PIO4_PRESS	= 0x41,
VALUE_PIO5_PRESS	= 0x51,
VALUE_PIO6_PRESS	= 0x61,
VALUE_PIO7_PRESS	= 0x71,
VALUE_PIO8_PRESS	= 0x81,
VALUE_PIO9_PRESS	= 0x91,
VALUE_PIO10_PRESS	= 0xa1,
VALUE_PIO11_PRESS	= 0xb1,
                        
VALUE_PIO0_LONG		= 0x09,
VALUE_PIO1_LONG		= 0x19,
VALUE_PIO2_LONG		= 0x29,
VALUE_PIO3_LONG		= 0x39,
VALUE_PIO4_LONG		= 0x49,
VALUE_PIO5_LONG		= 0x59,
VALUE_PIO6_LONG		= 0x69,
VALUE_PIO7_LONG		= 0x79,
VALUE_PIO8_LONG		= 0x89,
VALUE_PIO9_LONG		= 0x99,
VALUE_PIO10_LONG	= 0xa9,
VALUE_PIO11_LONG	= 0xb9,

VALUE_PIO0_RELEASE	= 0x00,
VALUE_PIO1_RELEASE	= 0x10,
VALUE_PIO2_RELEASE	= 0x20,
VALUE_PIO3_RELEASE	= 0x30,
VALUE_PIO4_RELEASE	= 0x40,
VALUE_PIO5_RELEASE	= 0x50,
VALUE_PIO6_RELEASE	= 0x60,
VALUE_PIO7_RELEASE	= 0x70,
VALUE_PIO8_RELEASE	= 0x80,
VALUE_PIO9_RELEASE	= 0x90,
VALUE_PIO10_RELEASE	= 0xa0,
VALUE_PIO11_RELEASE	= 0xb0
};


enum
{
    SPP_DEV_INQUIRY_TIMEOUT_IND = SPP_MSG_BASE,
    APP_PIO0_LONG,
    APP_PIO1_LONG,
    APP_PIO2_LONG,
    APP_PIO3_LONG,
    APP_PIO4_LONG,
    APP_PIO5_LONG,
    APP_PIO6_LONG,
    APP_PIO7_LONG,
    APP_PIO8_LONG,
    APP_PIO9_LONG,
    APP_PIO10_LONG,
    APP_PIO11_LONG,
    SPP_DEV_UDPATE_LED
};

typedef enum
{
    sppDevInitialising,
    sppDevReady,
    sppDevPairable,
    sppDevConnecting,
    sppDevConnected
} sppDevState;

typedef struct 
{
    TaskData            task;
    
    PioState            pio_state;
    SPP*                spp;
    bdaddr              bd_addr;
    sppDevState         spp_state;
    Sink                spp_sink;
    uint16              last_pio_state;
} sppTaskData;


/*************************************************************************
NAME    
    setSppState
    
DESCRIPTION
    Set the SPP State to the specified state

RETURNS
    
*/
void setSppState(const sppDevState state);


/****************************************************************************
NAME    
    sppAppTask
    
DESCRIPTION
  Returns the spp app's main task.

RETURNS
    Task
*/
Task getAppTask(void);


#endif /* _SPP_DEV_PRIVATE_H_ */


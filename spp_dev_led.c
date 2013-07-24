/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004-2006
Part of BlueLab 3.5.2-release

FILE NAME
    spp_dev_connect.c
    
DESCRIPTION
    Handles LED procedures of spp dev A application
    
*/

#include "spp_dev_private.h"
#include "spp_dev_led.h"

#include <pio.h>

#define LED_PAIRABLE_TIME   100
#define LED_CONNECTING_TIME 700


/****************************************************************************
NAME    
    sppDevLedInit
    
DESCRIPTION
    Initialise the LEDs

RETURNS
    void
*/
void sppDevLedInit()
{
    PioSetDir(LED, LED);
    PioSet(LED,0);
}

/****************************************************************************
NAME    
    sppDevUpdateLed
    
DESCRIPTION
    Update the LEDs

RETURNS
    void
*/
void sppDevUpdateLed(sppDevState state)
{
    bool on = (PioGet() & LED?TRUE:FALSE);
    
    MessageCancelAll(getAppTask(), SPP_DEV_UDPATE_LED);
    
    switch (state)
    {
    case sppDevInitialising:
        break;
    case sppDevReady:
        if (on)
            PioSet(LED, 0);
        break;
    case sppDevPairable:
        if (on)
            PioSet(LED, 0);
        else
            PioSet(LED, LED);
        MessageSendLater(getAppTask(), SPP_DEV_UDPATE_LED, 0, LED_PAIRABLE_TIME);
        break;
    case sppDevConnecting:
        if (on)
            PioSet(LED, 0);
        else
            PioSet(LED, LED);
        MessageSendLater(getAppTask(), SPP_DEV_UDPATE_LED, 0, LED_CONNECTING_TIME);
        break;
    case sppDevConnected:
        if (!on)
            PioSet(LED, LED);
        break;
    default:
        break;
    }
}

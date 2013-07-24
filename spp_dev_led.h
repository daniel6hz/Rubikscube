/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004-2006
Part of BlueLab 3.5.2-release

FILE NAME
    spp_dev_led.h
    
DESCRIPTION
    Handles LED procedures of spp dev A application
    
*/

#ifndef _SPP_DEV_LED_H_
#define _SPP_DEV_LED_H_

/****************************************************************************
NAME    
    sppDevLedInit
    
DESCRIPTION
    Initialise the LEDs

RETURNS
    void
*/
void sppDevLedInit(void);

/****************************************************************************
NAME    
    sppDevUpdateLed
    
DESCRIPTION
    Update the LEDs

RETURNS
    void
*/
void sppDevUpdateLed(sppDevState state);

#endif /* _SPP_DEV_LED_H_ */


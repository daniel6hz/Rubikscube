/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004-2006
Part of BlueLab 3.5.2-release

FILE NAME
    spp_dev_init.h
    
DESCRIPTION
    Handles initialisation procedures of spp dev A application
    
*/

/****************************************************************************
    Header files
*/
#include "spp_dev_init.h"
#include "spp_dev_private.h"

#include <spp.h>

/****************************************************************************
NAME    
    sppDevInit
    
DESCRIPTION
    Initialisation of Spp profile

RETURNS
    void
*/
void sppDevInit()
{
    /* Initialise the spp profile lib, stating that this is device B */ 
    SppInit(getAppTask(), devB, 255);
}

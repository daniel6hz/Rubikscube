/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004-2006
Part of BlueLab 3.5.2-release

FILE NAME
    spp_dev_inquire.h
    
DESCRIPTION
    Handles inquiry procedures of spp dev A application
    
*/

/****************************************************************************
    Header files
*/
#include "spp_dev_inquire.h"

#include <connection.h>


#define CLASS_OF_DEVICE		0x1F00


/****************************************************************************
NAME    
    sppDevInquire
    
DESCRIPTION
    Start Spp inquiry process

RETURNS
    void
*/
void sppDevInquire(sppTaskData* app)
{
    /* Turn off security */
    ConnectionSmRegisterIncomingService(0x0000, 0x0001, 0x0000);
    /* Write class of device */
    ConnectionWriteClassOfDevice(CLASS_OF_DEVICE);
    /* Start Inquiry mode */
    setSppState(sppDevPairable);
    /* Set devB device to inquiry scan mode, waiting for discovery */
    /*ConnectionWriteInquiryscanActivity(0x400, 0x200);*/
    ConnectionWriteInquiryscanActivity(0x800,0x0012);
    ConnectionSmSetSdpSecurityIn(TRUE);
    /* Make this device discoverable (inquiry scan), and connectable (page scan) */
    ConnectionWriteScanEnable(hci_scan_enable_inq_and_page);
        
}


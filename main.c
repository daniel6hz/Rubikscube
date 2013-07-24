/* Copyright (C) Cambridge Silicon Radio Ltd. 2005-2006 */
/* Part of BlueLab 3.5.2-release */
#include "spp_dev_private.h"
#include "spp_dev_init.h"
#include "spp_dev_inquire.h"
#include "spp_dev_auth.h"
#include "spp_dev_led.h"
#include "spp_button.h"

#include <connection.h>
#include <ps.h>
#include <panic.h>
#include <stdio.h>
#include <stream.h>
#include <pio.h>
#include <boot.h>
#include <string.h>

/*#define DEBUG_ENABLED*/

#ifdef DEBUG_ENABLED
#define DEBUG(x) {printf x;}
#else
#define DEBUG(x)
#endif

static sppTaskData theSppApp;



static void SppSendByte(uint8 byte)
{
    Sink uart = theSppApp.spp_sink;
    char * data = (char*)(&byte);
    if(uart)
    {
      if (SinkClaim(uart,1) != 0xFFFF) 
      {
        memcpy(SinkMap(uart),(char *)data,1);
        (void)PanicZero(SinkFlush(uart,1));
      }
    }
}

/*************************************************************************
NAME    
    sppAppTask
    
DESCRIPTION
  Returns the spp app's main task.

RETURNS
    Task
*/
Task getAppTask(void)
{
    return &theSppApp.task;
}

/*************************************************************************
NAME    
    unhandledSppState
    
DESCRIPTION
    This function is called when a message arrives and the Spp app is
    in an unexpected state.  
    
RETURNS
    
*/
static void unhandledSppState(sppDevState state, MessageId id)
{
    DEBUG(("SPP current state %d message id 0x%x\n", state, id));   
}

/*************************************************************************
NAME    
    setSppState
    
DESCRIPTION
    Set the SPP State to the specified state

RETURNS
    
*/
void setSppState(const sppDevState state)
{
    DEBUG(("SPP State - C=%d N=%d\n",theSppApp.spp_state, state));
    theSppApp.spp_state = state;
    /*sppDevUpdateLed(state);*/
}

/* Task handler function */
static void app_handler(Task task, MessageId id, Message message)
{
    sppDevState state = theSppApp.spp_state;
    
    
    switch(id)
    {
    case CL_INIT_CFM:
        DEBUG(("CL_INIT_CFM\n"));
        if(((CL_INIT_CFM_T*)message)->status == success)
            /* Connection Library initialisation was a success */
            sppDevInit();   
        else
            Panic();
        break;
    case SPP_INIT_CFM:
        DEBUG(("SPP_INIT_CFM\n"));
        switch(state)
        {
        case sppDevInitialising:
            /* Check for spp_init_success. What do we do if it failed? */
            if (((SPP_INIT_CFM_T *) message)->status == spp_init_success)
            {
                setSppState(sppDevReady);
                theSppApp.spp = ((SPP_INIT_CFM_T *) message)->spp;
                sppDevInquire(&theSppApp);
            }
            break;
        case sppDevReady:
        case sppDevPairable:
        case sppDevConnecting:
        case sppDevConnected:
        default:
            unhandledSppState(state, id);
            break;
        }
        break;
    case SPP_CONNECT_CFM:
        DEBUG(("SPP_CONNECT_CFM result = %d\n",((SPP_CONNECT_CFM_T*)message)->status));
        switch(state)
        {   
        case sppDevConnecting:
            /* Connect cfm, but must check status as connection may have failed */
            if (((SPP_CONNECT_CFM_T*)message)->status == rfcomm_connect_success)
            {
                theSppApp.spp_sink = ((SPP_CONNECT_CFM_T*)message)->sink;
                /* Connection Success */
                DEBUG(("Device connected...\n"));
                /* Connect Uart to Rfcomm */
                /*
        		(void) StreamConnect(StreamUartSource(), ((SPP_CONNECT_CFM_T*)message)->sink);
	            (void) StreamConnect(StreamSourceFromSink(((SPP_CONNECT_CFM_T*)message)->sink), StreamUartSink());
                */
                setSppState(sppDevConnected);
                ConnectionWriteScanEnable(hci_scan_enable_off);
                (void) MessageCancelFirst(&theSppApp.task, SPP_DEV_INQUIRY_TIMEOUT_IND);
                
            }
            else
            {
                /* Connection failed */
                setSppState(sppDevPairable);
                DEBUG(("Connection failed\n"));
            }
            break;
        case sppDevPairable:
            /* Connect cfm, but must check status as connection may have failed */
            if (((SPP_CONNECT_CFM_T*)message)->status == rfcomm_connect_success)
            {
                /* Device has been reset to pairable mode. Disconnect from current device */
                SppDisconnect(theSppApp.spp);
            }
            break;
        case sppDevInitialising:
        case sppDevReady:
        case sppDevConnected:
        default:
            unhandledSppState(state, id);
            break;
        }
        break;
    case SPP_CONNECT_IND:
        DEBUG(("SPP_CONNECT_IND\n"));
        switch(state)
        {   
        case sppDevPairable:
            /* Received command that a device is trying to connect. Send response. */
            sppDevAuthoriseConnectInd(&theSppApp,(SPP_CONNECT_IND_T*)message);
            setSppState(sppDevConnecting);
            
            theSppApp.spp_sink = 0;
            break;
        case sppDevInitialising:
        case sppDevConnecting:
        case sppDevReady:
        case sppDevConnected:
        default:
            unhandledSppState(state, id);
            break;
        }
        break;
    case SPP_DISCONNECT_IND:
        DEBUG(("SPP_DISCONNECT_IND\n"));
        /* Disconnect message has arrived */
        
        PioSet(PIO_CONNECTED,0);
        switch(state)
        {
        case sppDevConnected:
            sppDevInquire(&theSppApp);
            break;
        case sppDevInitialising:
        case sppDevPairable:
        case sppDevConnecting:
        case sppDevReady:
        default:
            unhandledSppState(state, id);
            break;
        }
        break;
        
    case SPP_DEV_INQUIRY_TIMEOUT_IND:
        DEBUG(("SPP_DEV_INQUIRY_TIMEOUT_IND\n"));
        switch(state)
        {
        case sppDevPairable:
            /* Inquiry mode timed out */
            ConnectionWriteScanEnable(hci_scan_enable_off);
            setSppState(sppDevReady);
            break;
        case sppDevConnected:
        case sppDevInitialising:
        case sppDevConnecting:
        case sppDevReady:
        default:
            unhandledSppState(state, id);
            break;
        }
        break;
    case SPP_DEV_UDPATE_LED:
        sppDevUpdateLed(theSppApp.spp_state);
        break;
        
    case CL_SM_PIN_CODE_IND:
        DEBUG(("CL_SM_PIN_CODE_IND\n"));
        sppDevHandlePinCodeRequest((CL_SM_PIN_CODE_IND_T *) message);
        break;
    case CL_SM_AUTHORISE_IND:  
        DEBUG(("CL_SM_PIN_CODE_IND\n"));
        sppDevAuthoriseResponse((CL_SM_AUTHORISE_IND_T*) message);
        break;
    case CL_SM_AUTHENTICATE_CFM:
        DEBUG(("CL_SM_AUTHENTICATE_CFM\n"));
        sppDevSetTrustLevel((CL_SM_AUTHENTICATE_CFM_T*)message);    
        break;
    case CL_DM_LINK_POLICY_IND:
        DEBUG(("CL_DM_LINK_POLICY_IND\n"));
        break;
    case SPP_MESSAGE_MORE_DATA:
        DEBUG(("SPP_MESSAGE_MORE_DATA\n"));
        break;
    case SPP_MESSAGE_MORE_SPACE:
        DEBUG(("SPP_MESSAGE_MORE_SPACE\n"));
        break;
    /*    
	case PIO1_PRESS:
		SppSendByte(VALUE_PIO5_LONG);
        break; 
		*/
        
      /*  
    case PIO0_PRESS:
        break;
	case PIO0_LONG:
        break;
	case PIO0_RELEASE:
        break;
	case PIO1_PRESS:
        break;
	case PIO1_LONG:
        break;
	case PIO1_RELEASE:
        break;
	case PIO2_PRESS:
        break;
	case PIO2_LONG:
        break;
	case PIO2_RELEASE:
        break;
	case PIO3_PRESS:
        break;
	case PIO3_LONG:
        break;
	case PIO3_RELEASE:
        break;
	case PIO4_PRESS:
        break;
	case PIO4_LONG:
        break;
	case PIO4_RELEASE:
        break;
	case PIO5_PRESS:
        break;
	case PIO5_LONG:
        break;
	case PIO5_RELEASE:
        break;
	case PIO6_PRESS:
        break;
	case PIO6_LONG:
        break;
	case PIO6_RELEASE:
        break;
	case PIO7_PRESS:
        break;
	case PIO7_LONG:
        break;
	case PIO8_PRESS:
        break;
	case PIO8_LONG:
        break;
	case PIO8_RELEASE:
        break;
	case PIO9_PRESS:
        break;
	case PIO9_LONG:
        break;
	case PIO9_RELEASE:
        break;
	case PIO10_PRESS:
        break;
	case PIO10_LONG:
        break;
	case PIO10_RELEASE:
        break;
	case PIO11_PRESS:
        break;
	case PIO11_LONG:
        break;
	case PIO11_RELEASE:
        break;
        */
   /*    
    case APP_PIO0_LONG:
        SppSendByte(VALUE_PIO0_LONG);
        break;
    case APP_PIO1_LONG:
        SppSendByte(VALUE_PIO1_LONG);
        break;
    case APP_PIO2_LONG:
        SppSendByte(VALUE_PIO2_LONG);
        break;
    case APP_PIO3_LONG:
        SppSendByte(VALUE_PIO3_LONG);
        break;
    case APP_PIO4_LONG:
        SppSendByte(VALUE_PIO4_LONG);
        break;
    case APP_PIO5_LONG:
        SppSendByte(VALUE_PIO5_LONG);
        break;
    case APP_PIO6_LONG:
        SppSendByte(VALUE_PIO6_LONG);
        break;
    case APP_PIO7_LONG:
        SppSendByte(VALUE_PIO7_LONG);
        break;
    case APP_PIO8_LONG:
        SppSendByte(VALUE_PIO8_LONG);
        break;
    case APP_PIO9_LONG:
        SppSendByte(VALUE_PIO9_LONG);
        break;
    case APP_PIO10_LONG:
        SppSendByte(VALUE_PIO10_LONG);
        break;
    case APP_PIO11_LONG:
        SppSendByte(VALUE_PIO11_LONG);
        break;
		*/
        
    case PIO_RAW:
        {
            uint16 pio = ((PIO_RAW_T*)message)->pio;
            uint8 i;
            for(i=0;i<12;i++)
            {
                if( pio & (1<<i) )
                {
                    if(theSppApp.last_pio_state & (1<<i))
                    {                        
                    }
                    else
                    {         
                        SppSendByte(VALUE_PIO0_PRESS+(i<<4));
                       /* MessageSendLater(getAppTask(),APP_PIO0_LONG+i,0,2000);*/
                    }
                }
                else
                {    
                    if(theSppApp.last_pio_state & (1<<i))
                        SppSendByte(VALUE_PIO0_RELEASE+(i<<4));
                    /*MessageCancelAll(getAppTask(),APP_PIO0_LONG+i);*/
                }
            }
            theSppApp.last_pio_state = pio;
        }
        break;
    default:
        /* An unexpected message has arrived - must handle it */
        DEBUG(("main app - msg type  not yet handled 0x%x\n", id));
        break;
    }
}
static void EncryptCode(void)
{
	/*
   uint16 addr[4],addr1[4],temp;
    
   if( PsFullRetrieve(0x025c,addr1, 4))
   {
	
		temp = addr1[3];
		addr1[3] = addr1[0];
		addr1[0] = temp;
		temp = addr1[2];
		addr1[2] = addr1[1];
		addr1[1] = temp;

		addr[0] = ((addr1[0] >> 5)&0x7ff)| (( addr1[3] <<11)& 0xf800);
		addr[1] = ((addr1[1] >> 5)&0x7ff)| (( addr1[0] <<11)& 0xf800);
		addr[2] = ((addr1[2] >> 5)&0x7ff)| (( addr1[1] <<11)& 0xf800);
    	addr[3] = ((addr1[3] >> 5)&0x7ff)| (( addr1[2] <<11)& 0xf800);

		PsFullRetrieve(0x0001,addr1,4);
   	 	if( (addr1[0]!=addr[0]) || (addr1[1] != addr[1]) || (addr1[2] != addr[2]) || (addr1[3] != addr[3]))
    		Panic();
	}
    else
		Panic();
        */
}
       
int main(void)
{
    DEBUG(("Main Started...\n"));
	
	EncryptCode();
#ifndef NO_UART_CHECK
    /* Make sure Uart has been successfully initialised before running */
    if (StreamUartSource())
#endif
    {

        /* Set up task 1 handler */
        theSppApp.task.handler = app_handler;
        
        setSppState(sppDevInitialising);
        
        sppDevLedInit();

        /* Init the Connection Manager */
        ConnectionInit(&theSppApp.task);
        
        
        pioInit(&theSppApp.pio_state, &theSppApp.task);

        /* Start the message scheduler loop */
        MessageLoop();
    }
    
    /* Will never get here! */
    DEBUG(("Main Ended!\n"));
    
    return 0;
}


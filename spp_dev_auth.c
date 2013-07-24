/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004-2006
Part of BlueLab 3.5.2-release

FILE NAME
    spp_dev_auth.h
    
DESCRIPTION
    Handles authorisation procedures of spp dev A application
    
*/

/****************************************************************************
    Header files
*/
#include "spp_dev_auth.h"
#include "spp_dev_private.h"

#include <stdio.h>
#include <ps.h>

/****************************************************************************
NAME    
    sppDevHandlePinCodeRequest
    
DESCRIPTION
    Reply to pin code request

RETURNS
    void
*/
void sppDevHandlePinCodeRequest(const CL_SM_PIN_CODE_IND_T* ind)
{
    uint16 pin_length = 0;
    uint8 pin[16];
	
   /* Do we have a fixed pin in PS, if not reject pairing (by setting the length to zero) */ 
    if ((pin_length = PsFullRetrieve(PSKEY_FIXED_PIN, pin, 16)) == 0 || pin_length > 16)
       pin_length = 0; 
   
    
    /* Respond to the PIN code request */
    ConnectionSmPinCodeResponse(&ind->bd_addr, pin_length, pin); 
}

/****************************************************************************
NAME    
    sppDevAuthoriseResponse
    
DESCRIPTION
    Give authorisation to device

RETURNS
    void
*/
void sppDevAuthoriseResponse(const CL_SM_AUTHORISE_IND_T* ind)
{
    ConnectionSmAuthoriseResponse(&ind->bd_addr, 
                                          ind->protocol_id, 
                                          ind->channel, 
                                          ind->incoming, 
                                          TRUE);
}

/****************************************************************************
NAME    
    sppDevAuthoriseConnectInd
    
DESCRIPTION
    Authorise a connect request

RETURNS
    void
*/
void sppDevAuthoriseConnectInd(sppTaskData* app, const SPP_CONNECT_IND_T* ind)
{
    SppConnectResponse(ind->spp, TRUE, &ind->addr);
}

/****************************************************************************
NAME    
    sppDevSetTrustLevel
    
DESCRIPTION
    Set the trust level of a device

RETURNS
    void
*/
void sppDevSetTrustLevel(const CL_SM_AUTHENTICATE_CFM_T* cfm)
{
    if(cfm->status == auth_status_success)
    {
        /* DEBUG("Pairing success, now set the trust level\n");*/
        ConnectionSmSetTrustLevel(&cfm->bd_addr, TRUE);
    }
    else if(cfm->status == auth_status_fail)
    {
        /* DEBUG("Pairing failed\n");*/
    }
    else
    {
        /* DEBUG("Pairing timeout\n");*/
    }
}

/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd. 2004-2006
Part of BlueLab 3.5.2-release

FILE NAME
    spp_dev_auth.h
    
DESCRIPTION
    Handles authorisation procedures of spp dev A application
    
*/

#ifndef _SPP_DEV_AUTH_H_
#define _SPP_DEV_AUTH_H_

#include "spp_dev_private.h"

#include <connection.h>

/****************************************************************************
NAME    
    sppDevHandlePinCodeRequest
    
DESCRIPTION
    Reply to pin code request

RETURNS
    void
*/
void sppDevHandlePinCodeRequest(const CL_SM_PIN_CODE_IND_T* ind);

/****************************************************************************
NAME    
    sppDevAuthoriseResponse
    
DESCRIPTION
    Give authorisation to device

RETURNS
    void
*/
void sppDevAuthoriseResponse(const CL_SM_AUTHORISE_IND_T* ind);

/****************************************************************************
NAME    
    sppDevAuthoriseConnectInd
    
DESCRIPTION
    Authorise a connect request

RETURNS
    void
*/
void sppDevAuthoriseConnectInd(sppTaskData* app, const SPP_CONNECT_IND_T* ind);

/****************************************************************************
NAME    
    sppDevSetTrustLevel
    
DESCRIPTION
    Set the trust level of a device

RETURNS
    void
*/
void sppDevSetTrustLevel(const CL_SM_AUTHENTICATE_CFM_T* cfm);

#endif /* _SPP_DEV_AUTH_H_ */

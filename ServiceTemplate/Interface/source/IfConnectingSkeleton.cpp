#include "../include/IfConnectingSkeleton.h"
#include "Public/include/TypeUnmarshaller.h"
#include "../include/IfConnectingMethodId.h"
namespace Zephyr 
{
TInt32 IfConnectingSkeleton::HandleMsg(CMessageHeader *pMsg)
{
    typedef TInt32 (IfConnectingSkeleton::*_PFMSG)(CMessageHeader *); 
    struct _MSGMAP_ENTRY { TUInt32 m_uMsgID; _PFMSG m_pHandlerFunc; };
    static _MSGMAP_ENTRY sMsgMapEntries[] = 
    {
        {(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|REGISTERSERVICE_CDOID_PT_TUINT32_TUINT32_TUINT32_TUINT32_ID), &IfConnectingSkeleton::HandleRegisterService_CDoid_pt_TUInt32_TUInt32_TUInt32_TUInt32},
        {(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|UNREGISTERSERVICE_TUINT32_TUINT32_TUINT32_ID), &IfConnectingSkeleton::HandleUnregisterService_TUInt32_TUInt32_TUInt32},
        {(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|REGISTERTEAM_TUINT32_ID), &IfConnectingSkeleton::HandleRegisterTeam_TUInt32},
        {(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|DISCONNECT_TUINT32_ID), &IfConnectingSkeleton::HandleDisconnect_TUInt32},
        {(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|SETID_TUINT32_ID), &IfConnectingSkeleton::HandleSetId_TUInt32},
        {(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|CHECKID_TUINT32_ID), &IfConnectingSkeleton::HandleCheckId_TUInt32},
    };
    TInt32 nBegin = 0;
    TInt32 nEnd = 6;
    TUInt32 nMethodId = pMsg->GetMethodId();
    _PFMSG pPfMsg = NULL;
    while(nBegin < nEnd)
    {
        if (nBegin == (nEnd -1))
        {
            if (sMsgMapEntries[nBegin].m_uMsgID == nMethodId)
            {
                pPfMsg=sMsgMapEntries[nBegin].m_pHandlerFunc;
            }
            else if (sMsgMapEntries[nEnd].m_uMsgID == nMethodId)
            {
                pPfMsg=sMsgMapEntries[nEnd].m_pHandlerFunc;
            }
            break;
        }
        TInt32 nMiddle = (nBegin + nEnd) >> 1;
        TUInt32 nMiddleVal = sMsgMapEntries[nMiddle].m_uMsgID;
        if (nMiddleVal == nMethodId)
        {
            pPfMsg = sMsgMapEntries[nMiddle].m_pHandlerFunc;
            break;
        }
        else
        {
            if (nMiddleVal>nMethodId)
            {
                nEnd = nMiddle;
            }
            else
            {
                nBegin   = nMiddle;
            }
        }
    }
    if (NULL == pPfMsg)
    {
        return CAN_NOT_HANDLE_THIS_MSG;
    }
    return (this->*pPfMsg)(pMsg);
}; 
TInt32 IfConnectingSkeleton::HandleRegisterService_CDoid_pt_TUInt32_TUInt32_TUInt32_TUInt32(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    CDoid* pDoid;
    nRet = Unmarshall(pBuffer,nLen,pDoid);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uServiceId;
    nRet = Unmarshall(pBuffer,nLen,uServiceId);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uServiceIdBegin;
    nRet = Unmarshall(pBuffer,nLen,uServiceIdBegin);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uServcieIdEnd;
    nRet = Unmarshall(pBuffer,nLen,uServcieIdEnd);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uPriority;
    nRet = Unmarshall(pBuffer,nLen,uPriority);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->RegisterService(pDoid,uServiceId,uServiceIdBegin,uServcieIdEnd,uPriority);
    return SUCCESS;
}
TInt32 IfConnectingSkeleton::HandleUnregisterService_TUInt32_TUInt32_TUInt32(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uServiceId;
    nRet = Unmarshall(pBuffer,nLen,uServiceId);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uServiceIdBegin;
    nRet = Unmarshall(pBuffer,nLen,uServiceIdBegin);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uServcieIdEnd;
    nRet = Unmarshall(pBuffer,nLen,uServcieIdEnd);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->UnregisterService(uServiceId,uServiceIdBegin,uServcieIdEnd);
    return SUCCESS;
}
TInt32 IfConnectingSkeleton::HandleRegisterTeam_TUInt32(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uTeamID;
    nRet = Unmarshall(pBuffer,nLen,uTeamID);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->RegisterTeam(uTeamID);
    return SUCCESS;
}
TInt32 IfConnectingSkeleton::HandleDisconnect_TUInt32(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uReason;
    nRet = Unmarshall(pBuffer,nLen,uReason);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->Disconnect(uReason);
    return SUCCESS;
}
TInt32 IfConnectingSkeleton::HandleSetId_TUInt32(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uId;
    nRet = Unmarshall(pBuffer,nLen,uId);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->SetId(uId);
    return SUCCESS;
}
TInt32 IfConnectingSkeleton::HandleCheckId_TUInt32(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uId;
    nRet = Unmarshall(pBuffer,nLen,uId);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->CheckId(uId);
    return SUCCESS;
}
}

#include "../include/IfGatewaySvcSkeleton.h"
#include "Public/include/TypeUnmarshaller.h"
#include "../include/IfGatewaySvcMethodId.h"
namespace Zephyr 
{
TInt32 IfGatewaySvcSkeleton::HandleMsg(CMessageHeader *pMsg)
{
    typedef TInt32 (IfGatewaySvcSkeleton::*_PFMSG)(CMessageHeader *); 
    struct _MSGMAP_ENTRY { TUInt32 m_uMsgID; _PFMSG m_pHandlerFunc; };
    static _MSGMAP_ENTRY sMsgMapEntries[] = 
    {
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|SYN2MAP_TUINT32_TLV_TPL_BEGIN_TUINT8_AND_TUINT16_TPL_END__REF_ID), &IfGatewaySvcSkeleton::HandleSyn2Map_TUInt32_TLV_tpl_begin_TUInt8_and_TUInt16_tpl_end__ref},
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|REGISTERSERVICE_TUINT32_TUINT32_TUINT32_TUINT32_OCTSEQ_TPL_BEGIN_TUINT16_TPL_END__REF_ID), &IfGatewaySvcSkeleton::HandleRegisterService_TUInt32_TUInt32_TUInt32_TUInt32_OctSeq_tpl_begin_TUInt16_tpl_end__ref},
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|UNREGISTERSERVICE_TUINT32_TUINT32_TUINT32_ID), &IfGatewaySvcSkeleton::HandleUnRegisterService_TUInt32_TUInt32_TUInt32},
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|BROADCASTTEAMMSG_TUINT32_OCTSEQ_TPL_BEGIN_TUINT16_TPL_END__REF_ID), &IfGatewaySvcSkeleton::HandleBroadcastTeamMsg_TUInt32_OctSeq_tpl_begin_TUInt16_tpl_end__ref},
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|CHANGEPRIORTY_TUINT32_CDOID_PT_TUINT32_ID), &IfGatewaySvcSkeleton::HandleChangePriorty_TUInt32_CDoid_pt_TUInt32},
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|STARTLOGIN_TUINT32_TUINT16_TUINT16_ID), &IfGatewaySvcSkeleton::HandleStartLogin_TUInt32_TUInt16_TUInt16},
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|STOPLOGIN_ID), &IfGatewaySvcSkeleton::HandleStopLogin},
        {(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|DISCONNECTEDALLCLIENT_ID), &IfGatewaySvcSkeleton::HandleDisconnectedAllClient},
    };
    TInt32 nBegin = 0;
    TInt32 nEnd = 8;
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
TInt32 IfGatewaySvcSkeleton::HandleSyn2Map_TUInt32_TLV_tpl_begin_TUInt8_and_TUInt16_tpl_end__ref(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uFrom;
    nRet = Unmarshall(pBuffer,nLen,uFrom);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TLV<TUInt8,TUInt16> tTLV;
    nRet = Unmarshall(pBuffer,nLen,tTLV);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->Syn2Map(uFrom,tTLV);
    return SUCCESS;
}
TInt32 IfGatewaySvcSkeleton::HandleRegisterService_TUInt32_TUInt32_TUInt32_TUInt32_OctSeq_tpl_begin_TUInt16_tpl_end__ref(CMessageHeader *pMsg)
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
    TUInt32 uServicBegin;
    nRet = Unmarshall(pBuffer,nLen,uServicBegin);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uEnd;
    nRet = Unmarshall(pBuffer,nLen,uEnd);
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
    OctSeq<TUInt16> tServiceName;
    nRet = Unmarshall(pBuffer,nLen,tServiceName);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->RegisterService(uServiceId,uServicBegin,uEnd,uPriority,tServiceName);
    return SUCCESS;
}
TInt32 IfGatewaySvcSkeleton::HandleUnRegisterService_TUInt32_TUInt32_TUInt32(CMessageHeader *pMsg)
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
    TUInt32 uServicBegin;
    nRet = Unmarshall(pBuffer,nLen,uServicBegin);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt32 uEnd;
    nRet = Unmarshall(pBuffer,nLen,uEnd);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->UnRegisterService(uServiceId,uServicBegin,uEnd);
    return SUCCESS;
}
TInt32 IfGatewaySvcSkeleton::HandleBroadcastTeamMsg_TUInt32_OctSeq_tpl_begin_TUInt16_tpl_end__ref(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uTeam;
    nRet = Unmarshall(pBuffer,nLen,uTeam);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    OctSeq<TUInt16> tServiceName;
    nRet = Unmarshall(pBuffer,nLen,tServiceName);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->BroadcastTeamMsg(uTeam,tServiceName);
    return SUCCESS;
}
TInt32 IfGatewaySvcSkeleton::HandleChangePriorty_TUInt32_CDoid_pt_TUInt32(CMessageHeader *pMsg)
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
    CDoid* pMyDoid;
    nRet = Unmarshall(pBuffer,nLen,pMyDoid);
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
    m_pImplementObj->ChangePriorty(uServiceId,pMyDoid,uPriority);
    return SUCCESS;
}
TInt32 IfGatewaySvcSkeleton::HandleStartLogin_TUInt32_TUInt16_TUInt16(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 uIp;
    nRet = Unmarshall(pBuffer,nLen,uIp);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt16 nListeningPort;
    nRet = Unmarshall(pBuffer,nLen,nListeningPort);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TUInt16 nMaxConnection;
    nRet = Unmarshall(pBuffer,nLen,nMaxConnection);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->StartLogin(uIp,nListeningPort,nMaxConnection);
    return SUCCESS;
}
TInt32 IfGatewaySvcSkeleton::HandleStopLogin(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    m_pImplementObj->StopLogin();
    return SUCCESS;
}
TInt32 IfGatewaySvcSkeleton::HandleDisconnectedAllClient(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    m_pImplementObj->DisconnectedAllClient();
    return SUCCESS;
}
}

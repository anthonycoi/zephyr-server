#include "../include/IfConnectingStub.h"
#include "Public/include/TypeMarshaller.h"
#include "../include/IfConnectingMethodId.h"
namespace Zephyr 
{
TInt32 IfConnectingStub::RegisterService(CDoid* pDoid,TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd,TUInt32 uPriority)
{
    TInt32 nLen = sizeof(CDoid)+sizeof(TUInt32)+sizeof(TUInt32)+sizeof(TUInt32)+sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|REGISTERSERVICE_CDOID_PT_TUINT32_TUINT32_TUINT32_TUINT32_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,pDoid);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nRet = Marshall(pBuffer+nUsed,nLen,uServiceId);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nRet = Marshall(pBuffer+nUsed,nLen,uServiceIdBegin);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nRet = Marshall(pBuffer+nUsed,nLen,uServcieIdEnd);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nRet = Marshall(pBuffer+nUsed,nLen,uPriority);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    pMsg->ResetBodyLength(nUsed);
    return m_pOnwerObj->SendMsg(pMsg);
}

TInt32 IfConnectingStub::UnregisterService(TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TUInt32)+sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|UNREGISTERSERVICE_TUINT32_TUINT32_TUINT32_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uServiceId);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nRet = Marshall(pBuffer+nUsed,nLen,uServiceIdBegin);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nRet = Marshall(pBuffer+nUsed,nLen,uServcieIdEnd);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    pMsg->ResetBodyLength(nUsed);
    return m_pOnwerObj->SendMsg(pMsg);
}

TInt32 IfConnectingStub::RegisterTeam(TUInt32 uTeamID)
{
    TInt32 nLen = sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|REGISTERTEAM_TUINT32_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uTeamID);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    pMsg->ResetBodyLength(nUsed);
    return m_pOnwerObj->SendMsg(pMsg);
}

TInt32 IfConnectingStub::Disconnect(TUInt32 uReason)
{
    TInt32 nLen = sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|DISCONNECT_TUINT32_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uReason);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    pMsg->ResetBodyLength(nUsed);
    return m_pOnwerObj->SendMsg(pMsg);
}

TInt32 IfConnectingStub::SetId(TUInt32 uId)
{
    TInt32 nLen = sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|SETID_TUINT32_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uId);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    pMsg->ResetBodyLength(nUsed);
    return m_pOnwerObj->SendMsg(pMsg);
}

TInt32 IfConnectingStub::CheckId(TUInt32 uId)
{
    TInt32 nLen = sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFCONNECTING_INTERFACE_ID|CHECKID_TUINT32_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uId);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    nUsed += nRet;
    nLen-=nRet;
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    pMsg->ResetBodyLength(nUsed);
    return m_pOnwerObj->SendMsg(pMsg);
}

}

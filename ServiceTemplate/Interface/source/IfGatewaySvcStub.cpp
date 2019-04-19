#include "../include/IfGatewaySvcStub.h"
#include "Public/include/TypeMarshaller.h"
#include "../include/IfGatewaySvcMethodId.h"
namespace Zephyr 
{
TInt32 IfGatewaySvcStub::Syn2Map(TUInt32 uFrom,TLV<TUInt8,TUInt16>& tTLV)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TLV<TUInt8,TUInt16>);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|SYN2MAP_TUINT32_TLV_TPL_BEGIN_TUINT8_AND_TUINT16_TPL_END__REF_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uFrom);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tTLV);
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

TInt32 IfGatewaySvcStub::RegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd,TUInt32 uPriority,OctSeq<TUInt16>& tServiceName)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TUInt32)+sizeof(TUInt32)+sizeof(TUInt32)+sizeof(OctSeq<TUInt16>);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|REGISTERSERVICE_TUINT32_TUINT32_TUINT32_TUINT32_OCTSEQ_TPL_BEGIN_TUINT16_TPL_END__REF_ID),&m_tTarget,1,false);
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
    nRet = Marshall(pBuffer+nUsed,nLen,uServicBegin);
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
    nRet = Marshall(pBuffer+nUsed,nLen,uEnd);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tServiceName);
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

TInt32 IfGatewaySvcStub::UnRegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TUInt32)+sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|UNREGISTERSERVICE_TUINT32_TUINT32_TUINT32_ID),&m_tTarget,1,false);
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
    nRet = Marshall(pBuffer+nUsed,nLen,uServicBegin);
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
    nRet = Marshall(pBuffer+nUsed,nLen,uEnd);
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

TInt32 IfGatewaySvcStub::BroadcastTeamMsg(TUInt32 uTeam,OctSeq<TUInt16>& tServiceName)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(OctSeq<TUInt16>);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|BROADCASTTEAMMSG_TUINT32_OCTSEQ_TPL_BEGIN_TUINT16_TPL_END__REF_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uTeam);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tServiceName);
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

TInt32 IfGatewaySvcStub::ChangePriorty(TUInt32 uServiceId,CDoid* pMyDoid,TUInt32 uPriority)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(CDoid)+sizeof(TUInt32);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|CHANGEPRIORTY_TUINT32_CDOID_PT_TUINT32_ID),&m_tTarget,1,false);
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
    nRet = Marshall(pBuffer+nUsed,nLen,pMyDoid);
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

TInt32 IfGatewaySvcStub::StartLogin(TUInt32 uIp,TUInt16 nListeningPort,TUInt16 nMaxConnection)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TUInt16)+sizeof(TUInt16);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|STARTLOGIN_TUINT32_TUINT16_TUINT16_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,uIp);
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
    nRet = Marshall(pBuffer+nUsed,nLen,nListeningPort);
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
    nRet = Marshall(pBuffer+nUsed,nLen,nMaxConnection);
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

TInt32 IfGatewaySvcStub::StopLogin()
{
    TInt32 nLen = 0;
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|STOPLOGIN_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    return m_pOnwerObj->SendMsg(pMsg);
}

TInt32 IfGatewaySvcStub::DisconnectedAllClient()
{
    TInt32 nLen = 0;
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(GATEWAY_SERVICE_ID|IFGATEWAYSVC_INTERFACE_ID|DISCONNECTEDALLCLIENT_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    return m_pOnwerObj->SendMsg(pMsg);
}

}

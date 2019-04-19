#include "../include/IfControlServiceStub.h"
#include "Public/include/TypeMarshaller.h"
#include "../include/IfControlServiceMethodId.h"
namespace Zephyr 
{
TInt32 IfControlServiceStub::StartService(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tConfigs)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TLV<TUInt32,TUInt32>);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(SERVICE_CENTER_SERVICE_ID|IFCONTROLSERVICE_INTERFACE_ID|STARTSERVICE_TUINT32_TLV_TPL_BEGIN_TUINT32_AND_TUINT32_TPL_END__ID),&m_tTarget,1,false);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tConfigs);
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

TInt32 IfControlServiceStub::Command(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tCommands)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TLV<TUInt32,TUInt32>);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(SERVICE_CENTER_SERVICE_ID|IFCONTROLSERVICE_INTERFACE_ID|COMMAND_TUINT32_TLV_TPL_BEGIN_TUINT32_AND_TUINT32_TPL_END__ID),&m_tTarget,1,false);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tCommands);
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

TInt32 IfControlServiceStub::StopService(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tDatas)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TLV<TUInt32,TUInt32>);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(SERVICE_CENTER_SERVICE_ID|IFCONTROLSERVICE_INTERFACE_ID|STOPSERVICE_TUINT32_TLV_TPL_BEGIN_TUINT32_AND_TUINT32_TPL_END__ID),&m_tTarget,1,false);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tDatas);
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

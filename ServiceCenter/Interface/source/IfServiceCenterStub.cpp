#include "../include/IfServiceCenterStub.h"
#include "Public/include/TypeMarshaller.h"
#include "../include/IfServiceCenterMethodId.h"
namespace Zephyr 
{
TInt32 IfServiceCenterStub::OnSerivceStarted(TUInt32 nServiceId,const TChar* pszName,CDoid tDoid)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(TChar)+sizeof(CDoid);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(SERVICE_CENTER_SERVICE_ID|IFSERVICECENTER_INTERFACE_ID|ONSERIVCESTARTED_TUINT32_CONST_TCHAR_PT_CDOID_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,nServiceId);
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
    nRet = Marshall(pBuffer+nUsed,nLen,pszName);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tDoid);
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

TInt32 IfServiceCenterStub::OnServiceStoped(TUInt32 nServiceId,CDoid tDoid)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(CDoid);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(SERVICE_CENTER_SERVICE_ID|IFSERVICECENTER_INTERFACE_ID|ONSERVICESTOPED_TUINT32_CDOID_ID),&m_tTarget,1,false);
    if (NULL == pMsg)
    {
        return OUT_OF_MEM;
    }
    TUInt32 nUsed=0;
    TInt32 nRet=0;
    TUChar *pBuffer = pMsg->GetBody();
    nRet = Marshall(pBuffer+nUsed,nLen,nServiceId);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tDoid);
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

TInt32 IfServiceCenterStub::ServiceStatic(TUInt32 uServiceId,CDoid tDoid,TLV<TUInt16,TUInt16> tDatas)
{
    TInt32 nLen = sizeof(TUInt32)+sizeof(CDoid)+sizeof(TLV<TUInt16,TUInt16>);
    CMessageHeader *pMsg = m_pOnwerObj->PrepareMsg(nLen,(SERVICE_CENTER_SERVICE_ID|IFSERVICECENTER_INTERFACE_ID|SERVICESTATIC_TUINT32_CDOID_TLV_TPL_BEGIN_TUINT16_AND_TUINT16_TPL_END__ID),&m_tTarget,1,false);
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
    nRet = Marshall(pBuffer+nUsed,nLen,tDoid);
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

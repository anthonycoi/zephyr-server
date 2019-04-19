#include "../include/IfServiceCenterSkeleton.h"
#include "Public/include/TypeUnmarshaller.h"
#include "../include/IfServiceCenterMethodId.h"
namespace Zephyr 
{
TInt32 IfServiceCenterSkeleton::HandleMsg(CMessageHeader *pMsg)
{
    typedef TInt32 (IfServiceCenterSkeleton::*_PFMSG)(CMessageHeader *); 
    struct _MSGMAP_ENTRY { TUInt32 m_uMsgID; _PFMSG m_pHandlerFunc; };
    static _MSGMAP_ENTRY sMsgMapEntries[] = 
    {
        {(SERVICE_CENTER_SERVICE_ID|IFSERVICECENTER_INTERFACE_ID|ONSERIVCESTARTED_TUINT32_CONST_TCHAR_PT_CDOID_ID), &IfServiceCenterSkeleton::HandleOnSerivceStarted_TUInt32_const_TChar_pt_CDoid},
        {(SERVICE_CENTER_SERVICE_ID|IFSERVICECENTER_INTERFACE_ID|ONSERVICESTOPED_TUINT32_CDOID_ID), &IfServiceCenterSkeleton::HandleOnServiceStoped_TUInt32_CDoid},
        {(SERVICE_CENTER_SERVICE_ID|IFSERVICECENTER_INTERFACE_ID|SERVICESTATIC_TUINT32_CDOID_TLV_TPL_BEGIN_TUINT16_AND_TUINT16_TPL_END__ID), &IfServiceCenterSkeleton::HandleServiceStatic_TUInt32_CDoid_TLV_tpl_begin_TUInt16_and_TUInt16_tpl_end_},
    };
    TInt32 nBegin = 0;
    TInt32 nEnd = 3;
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
TInt32 IfServiceCenterSkeleton::HandleOnSerivceStarted_TUInt32_const_TChar_pt_CDoid(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 nServiceId;
    nRet = Unmarshall(pBuffer,nLen,nServiceId);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TChar* pszName;
    nRet = Unmarshall(pBuffer,nLen,pszName);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    CDoid tDoid;
    nRet = Unmarshall(pBuffer,nLen,tDoid);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->OnSerivceStarted(nServiceId,pszName,tDoid);
    return SUCCESS;
}
TInt32 IfServiceCenterSkeleton::HandleOnServiceStoped_TUInt32_CDoid(CMessageHeader *pMsg)
{
    TInt32 nLen = pMsg->GetBodyLength();
    TUChar *pBuffer =pMsg->GetBody();
    TInt32 nRet;
    TUInt32 nServiceId;
    nRet = Unmarshall(pBuffer,nLen,nServiceId);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    CDoid tDoid;
    nRet = Unmarshall(pBuffer,nLen,tDoid);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->OnServiceStoped(nServiceId,tDoid);
    return SUCCESS;
}
TInt32 IfServiceCenterSkeleton::HandleServiceStatic_TUInt32_CDoid_TLV_tpl_begin_TUInt16_and_TUInt16_tpl_end_(CMessageHeader *pMsg)
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
    CDoid tDoid;
    nRet = Unmarshall(pBuffer,nLen,tDoid);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    TLV<TUInt16,TUInt16> tDatas;
    nRet = Unmarshall(pBuffer,nLen,tDatas);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->ServiceStatic(uServiceId,tDoid,tDatas);
    return SUCCESS;
}
}

#include "../include/IfControlServiceSkeleton.h"
#include "Public/include/TypeUnmarshaller.h"
#include "../include/IfControlServiceMethodId.h"
namespace Zephyr 
{
TInt32 IfControlServiceSkeleton::HandleMsg(CMessageHeader *pMsg)
{
    typedef TInt32 (IfControlServiceSkeleton::*_PFMSG)(CMessageHeader *); 
    struct _MSGMAP_ENTRY { TUInt32 m_uMsgID; _PFMSG m_pHandlerFunc; };
    static _MSGMAP_ENTRY sMsgMapEntries[] = 
    {
        {(SERVICE_CENTER_SERVICE_ID|IFCONTROLSERVICE_INTERFACE_ID|STARTSERVICE_TUINT32_TLV_TPL_BEGIN_TUINT32_AND_TUINT32_TPL_END__ID), &IfControlServiceSkeleton::HandleStartService_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_},
        {(SERVICE_CENTER_SERVICE_ID|IFCONTROLSERVICE_INTERFACE_ID|COMMAND_TUINT32_TLV_TPL_BEGIN_TUINT32_AND_TUINT32_TPL_END__ID), &IfControlServiceSkeleton::HandleCommand_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_},
        {(SERVICE_CENTER_SERVICE_ID|IFCONTROLSERVICE_INTERFACE_ID|STOPSERVICE_TUINT32_TLV_TPL_BEGIN_TUINT32_AND_TUINT32_TPL_END__ID), &IfControlServiceSkeleton::HandleStopService_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_},
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
TInt32 IfControlServiceSkeleton::HandleStartService_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_(CMessageHeader *pMsg)
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
    TLV<TUInt32,TUInt32> tConfigs;
    nRet = Unmarshall(pBuffer,nLen,tConfigs);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->StartService(uServiceId,tConfigs);
    return SUCCESS;
}
TInt32 IfControlServiceSkeleton::HandleCommand_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_(CMessageHeader *pMsg)
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
    TLV<TUInt32,TUInt32> tCommands;
    nRet = Unmarshall(pBuffer,nLen,tCommands);
    if (nRet<SUCCESS)
    {
        pBuffer += nRet;
        nLen -= nRet;
    }
    else
    {
        return nRet;
    }
    m_pImplementObj->Command(uServiceId,tCommands);
    return SUCCESS;
}
TInt32 IfControlServiceSkeleton::HandleStopService_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_(CMessageHeader *pMsg)
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
    TLV<TUInt32,TUInt32> tDatas;
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
    m_pImplementObj->StopService(uServiceId,tDatas);
    return SUCCESS;
}
}

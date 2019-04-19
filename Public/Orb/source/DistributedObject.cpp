#include "../include/DistributedObject.h"

namespace Zephyr
{

CObject::CObject()
{
    m_pSkeleton = NULL;
    m_pCurrentMsg = NULL;

    m_nMsgHandled = 0;
    m_nMsgSent = 0;
    m_nMsgDroped = 0;
    m_nStartTime = 0;
}

TInt32      CObject::OnInit()
{
    m_nStartTime = 0;
    return SUCCESS;
}

TInt32 CObject::OnFinal()
{
    return SUCCESS;
}

CDoid *CObject::GetMyDoid()
{
    if (m_pSkeleton)
    {
        return m_pSkeleton->GetMyDoid();
    }
    return NULL;
}
//获得后自己初始化
CMessageHeader *CObject::PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid* destDoid,TInt32 destDoidNum,bool bRearrangeDest)
{
    if (m_pSkeleton)
    {
        return m_pSkeleton->PrepareMsg(bodyLength,methodId,destDoid,destDoidNum,bRearrangeDest);
    }
    return NULL;
}
//发送消息
TInt32  CObject::SendMsg(CMessageHeader *pMsg)
{
    if (m_pSkeleton)
    {
        return m_pSkeleton->SendMsg(pMsg);
    }
    return NULL_POINTER;
}

}
#include "../include/Skeleton.h"

namespace Zephyr
{

CSkeleton::CSkeleton()
{
    m_pRegisteredObj = NULL;
    m_pIfComm = NULL;
}
void CSkeleton::OnReused(TUInt16 uStep)
{
    m_tDoid.m_objId += uStep;
}

CDoid *CSkeleton::GetMyDoid()
{
#ifdef _DEBUG
    if (m_pRegisteredObj)
    {
#endif
        return &m_tDoid;
#ifdef _DEBUG
    }
    return NULL;
#endif
}
    //��ú��Լ���ʼ��
CMessageHeader *CSkeleton::PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid* destDoid,TInt32 destDoidNum,bool bRearrangeDest) 
{
    if (m_pRegisteredObj)
    {
        return m_pIfComm->PrepareMsg(bodyLength,methodId,m_tDoid,destDoid,destDoidNum,bRearrangeDest);
    }
    //û��ע�᲻��ʹ�ã�
    return NULL;
}

    //������Ϣ
TInt32 CSkeleton::SendMsg(CMessageHeader *pMsg)
{
    if (m_pRegisteredObj)
    {
        return m_pIfComm->SendMsg(pMsg);
    }
    return OBJ_NOT_REISTERED;
}


TInt32 CSkeleton::Init(IfCommunicator *pIfComm,CDoid *pDoid)
{
    m_pRegisteredObj = NULL;
    m_pIfComm = pIfComm;
    m_tDoid = *pDoid;
    return SUCCESS;
}


}

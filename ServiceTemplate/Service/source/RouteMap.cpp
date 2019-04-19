#include "../include/RouteMap.h"
#include "Public/include/Message.h"
#include "../include/GatewayLogger.h"

namespace Zephyr
{


CDoid *CRouteMap::FindService(TUInt32 uServiceId)
{
    TUInt32 uSvr = CMessageHeader::GetServiceID(uServiceId);
    TplMultiKeyMapNode<CRoute,TUInt32>::Iterator it = m_tServiceRoute.GetItemByKey(uSvr);
    CRoute *pRount = it;
    CRoute *pRtn(NULL);
    while ((pRount)&&(pRount->m_uKey == uSvr))
    {
        if ((uServiceId <= pRount->m_uIdBegin)&&(uServiceId> pRount->m_uIdEnd))
        {
            if (pRtn)
            {
                if (pRtn->m_uPriority > pRount->m_uPriority) //����Ȩ��.
                {
                    pRtn = pRount;
                }
            }
            else
            {
                pRtn = pRount;
            }
        }
        ++it;
    }
    if (pRtn)
    {
        return &pRtn->m_tRouteTo;
    }
    return NULL;
}

TInt32 CRouteMap::ChangePriorty(TUInt32 uServiceId,CDoid *pMyDoid,TUInt32 uPriority)
{
    TUInt32 uSvr = CMessageHeader::GetServiceID(uServiceId); //�޸�һ��Service��
    TplMultiKeyMapNode<CRoute,TUInt32>::Iterator it = m_tServiceRoute.GetItemByKey(uSvr);
    CRoute *pRount = it;
    TInt32 nCount = 0;
    while ((pRount)&&(pRount->m_uKey == uSvr))
    {
        if (pRount->m_tRouteTo == (*pMyDoid))
        {
            pRount->m_uPriority = uPriority;
            ++nCount;
        }
        ++it;
    }
    return nCount;
}

TInt32 CRouteMap::AddRoute(CDoid *pDoid,TUInt32 uSrvId,TUInt32 uBegin,TUInt32 uEnd,TUInt32 uPriority)
{
    if ((uSrvId != CMessageHeader::GetServiceID(uBegin))||(uSrvId!=CMessageHeader::GetServiceID(uEnd-1)))
    {
        //��δ���д��־
//         char szBufferRegister[64];
//         pRegister->ToStr(szBufferRegister);
//         char szBufferDoid[64];
//         pDoid->ToStr(szBufferDoid);
//         LOG_RUN(en_error_service_id,"Error Servcie Id,Register:%s ,doid:%s, uSrvId:%u,uBegin:%u,uEnd:%u,uPriority:%u",szBufferRegister,szBufferDoid,uSrvId,uBegin,uEnd,uPriority);
        return -((TInt32)en_error_service_id);
    }
    //������û�У�û�Ļ���˵.
    TplMultiKeyMapNode<CRoute,TUInt32>::Iterator it = m_tServiceRoute.GetItemByKey(uSrvId);
    CRoute *pRount = it;
    if (pRount)
    {
        CRoute tNew;
        tNew.m_tRouteTo = *pDoid;
        tNew.m_uIdBegin = uBegin;
        tNew.m_uIdEnd   = uEnd;
        tNew.m_uKey     = uSrvId;
        tNew.m_uPriority = uPriority;
        CRoute *pMerge = &tNew;
        while ((pRount)&&(pRount->m_uKey == uSrvId))
        {
            if (pRount->Merge(*pMerge)) //�ɹ��ں�
            {
                if (&tNew == pMerge)
                {
                    pMerge = pRount;
                }
                else
                {
                    //���ϵ�ɾ��.��Ϊ�Ѿ��ڵ�pRount����
                    m_tServiceRoute.RemoveFromTreeItem(pMerge);
                    pMerge = pRount;
                }
            }
            ++it;
        }
        if (&tNew != pMerge)
        {
            //���ˣ������Ѿ������ˣ����Է�����
            return SUCCESS;
        }
    }

    CRoute *pRoute = m_tServiceRoute.PrepareItem();
    if (pRoute)
    {
        pRoute->m_uKey = uSrvId;//CMessageHeader::GetServiceID();������
        pRoute->m_uIdBegin = uBegin;
        pRoute->m_uIdEnd = uEnd;
        pRoute->m_tRouteTo = *pDoid;
        pRoute->m_uPriority = uPriority;
        m_tServiceRoute.AddInTree(pRoute);
    }
    else
    {
        //д��־.�ڴ�
//         char szBufferRegister[64];
//         pRegister->ToStr(szBufferRegister);
//         char szBufferDoid[64];
//         pDoid->ToStr(szBufferDoid);
//         LOG_CRITICAL(en_allocate_route_mem_failed,"Servcie Id,Register:%s ,doid:%s, uSrvId:%u,uBegin:%u,uEnd:%u,uPriority:%u",szBufferRegister,szBufferDoid,uSrvId,uBegin,uEnd,uPriority);
        return OUT_OF_MEM;
    }
    return SUCCESS;
}

TInt32 CRouteMap::RmvRoute(CDoid *pDoid,TUInt32 uSrvId,TUInt32 uBegin,TUInt32 uEnd)
{
    if ((uSrvId != CMessageHeader::GetServiceID(uBegin))||(uSrvId!=CMessageHeader::GetServiceID(uEnd-1)))
    {
        //��δ���д��־
        //         char szBufferRegister[64];
        //         pRegister->ToStr(szBufferRegister);
        //         char szBufferDoid[64];
        //         pDoid->ToStr(szBufferDoid);
        //         LOG_RUN(en_error_service_id,"Error Servcie Id,Register:%s ,doid:%s, uSrvId:%u,uBegin:%u,uEnd:%u,uPriority:%u",szBufferRegister,szBufferDoid,uSrvId,uBegin,uEnd,uPriority);
        return -((TInt32)en_error_service_id);
    }

    TplMultiKeyMapNode<CRoute,TUInt32>::Iterator it = m_tServiceRoute.GetItemByKey(uSrvId);
    CRoute *pRount = it;
    TInt32 nResult = -((TInt32)en_no_much_service_id_found);
    if (pRount)
    {
        CRoute tRmvPart;
        tRmvPart.m_tRouteTo = *pDoid;
        tRmvPart.m_uIdBegin = uBegin;
        tRmvPart.m_uIdEnd   = uEnd;
        tRmvPart.m_uKey     = uSrvId;
        //tNew.m_uPriority = 0;�Ȳ�д
        
        while ((pRount)&&(pRount->m_uKey == uSrvId))
        {
            TInt32 nRet = pRount->Separate(tRmvPart);
            switch (nRet)
            {
            case 0:
                {
                    nResult = 0; 
                }
                break;
            case 3:
                {
                    //ɾ��������
                    m_tServiceRoute.RemoveFromTreeItem(pRount);
                    m_tServiceRoute.ReleaseItem(pRount);
                    return 3;
                }
                break;
            case 1: //����
                {
                    m_tServiceRoute.RemoveFromTreeItem(pRount);
                    m_tServiceRoute.ReleaseItem(pRount);
                    nResult = 0;
                }
                break;
            case 2:
                {
                    //���Ӳ�ֹͣ
                    CRoute *pNew = m_tServiceRoute.PrepareItem();
                    if (pNew)
                    {
                        pNew->m_tRouteTo = tRmvPart.m_tRouteTo;
                        pNew->m_uKey = tRmvPart.m_uKey;
                        pNew->m_uIdBegin = tRmvPart.m_uIdBegin;
                        pNew->m_uIdEnd = tRmvPart.m_uIdEnd;
                        pNew->m_uPriority = pRount->m_uPriority;
                        m_tServiceRoute.AddInTree(pNew);
                        return 2;
                    }
                    else
                    {
                        return OUT_OF_MEM;
                    }
                }
                break;
            }
            ++it;
            pRount = it;
        }
    }

    return nResult;
}


void CRouteMap::OnFinal()
{
    m_tServiceRoute.clean();
}

}
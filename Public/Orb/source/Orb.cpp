#include "../include/Orb.h"
#include "../../include/SysMacros.h"
#include <iostream>
#include "../../../System/include/SysInc.h"
#include "../include/OrbLogger.h"
namespace Zephyr
{

COrb::COrb()
{
    m_pIfComm = NULL;
    //IfObj          *m_pRunObj;
    m_nRunGapInMs = 0;
    m_nLastCheckTime = 0;
    m_nLocalNodeId = 0;
    m_nLocalVIP = 0;
    //这个Orb的service从这个开始
    m_nLocalServiceId = 0;

    m_nLocalServiceIdEnd = 0;

    m_pClock = NULL;
    for (int i=0;i<MAX_SERVICE_NR;++i)
    {
        m_ppServiceSkeleton[i] = NULL;
    }
    //需要run的都放这里,40ms跑一次
    m_nLastRunTime = 0;

    m_pLogger = NULL;

     m_nService2Stop = 0xFFFFFFFF;
}

COrb::~COrb()
{
    //其实没啥必要了
}

IfSkeleton* COrb::RegisterObj(IfObj *pObjSkeleton,TInt16 nSrvId)
{
    if ((nSrvId >= m_nLocalServiceId) && (nSrvId < (m_nLocalServiceIdEnd)))
    {
        CListNode<CArrayPoolNode<CSkeleton> >  *pRtn = m_tSkeletonPool.GetMem();
        if (pRtn)
        {
            pRtn->m_pListBelongsTo = NULL;
            pRtn->m_tDoid.m_srvId = nSrvId;
        }
        LOG_DEBUG(en_obj_registered,"Obj [%d,%d]",(TUInt32)nSrvId,(TUInt32)pRtn->m_tDoid.m_objId);
        return pRtn;
    }
    return NULL;
}

IfSkeleton* COrb::RegiterService(IfObj *pObj,TInt16 nSrvId)
{
    if ((nSrvId >= m_nLocalServiceId) && (nSrvId < (m_nLocalServiceIdEnd)))
    {
        if (m_ppServiceSkeleton[nSrvId-m_nLocalServiceId])
        {
            return NULL;
        }
        CListNode<CArrayPoolNode<CSkeleton> >  *pRtn = m_tSkeletonPool.GetMem();
        if (pRtn)
        {
            pRtn->m_pListBelongsTo = NULL;
            pRtn->OnInit();
            pRtn->RegisterObj(pObj,nSrvId);
            
            m_ppServiceSkeleton[nSrvId-m_nLocalServiceId] = pRtn;
        }
        LOG_RUN(en_service_registered,"Service %d Registered!",(TUInt32)nSrvId);
        return pRtn;
    }
    return NULL;
}

    //注销
void    COrb::UnRegisterObj(IfSkeleton *pIfSkel)
{
    CListNode<CArrayPoolNode<CSkeleton> > *pSk = (CListNode<CArrayPoolNode<CSkeleton> >*)pIfSkel;
    pSk->OnFinal();
    if (pSk->m_pListBelongsTo)
    {
        pSk->m_pListBelongsTo->Detach(pSk);
        pSk->m_pListBelongsTo = NULL;
    }
    pSk->OnReused(m_tSkeletonPool.GetMaxSize());
    m_tSkeletonPool.ReleaseMem(pSk);
}

TInt32 COrb::Init(IfCommunicator *pIfCom,CDoid *pDoidBegin,TInt32 nSkeletonNr,IfLogger *pLogger)
{
    m_pClock = pIfCom->GetClock();
    if (!m_pClock)
    {
        printf("Can not get clock!");
        return NULL_POINTER;
    }
    m_nLastCheckTime = m_pClock->GetLocalTime();
    m_pIfComm = pIfCom;

    m_nLocalNodeId = pDoidBegin->m_nodeId;
    m_nLocalVIP = pDoidBegin->m_virtualIp;
    //这个Orb的service从这个开始
    m_nLocalServiceId = pDoidBegin->m_srvId;
    m_pLogger = pLogger;

    m_nLocalServiceIdEnd = m_nLocalServiceId + MAX_SERVICE_NR_PER_COMM;
    TInt32 nRet = m_tSkeletonPool.InitPool(nSkeletonNr);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    CDoid tDoid = *pDoidBegin;
    tDoid.m_objId = 0;
    for (int i=0;i<nSkeletonNr;++i)
    {
        CSkeleton *pSkt = m_tSkeletonPool.FindMem(i);
        tDoid.m_objId = i;
        pSkt->Init(pIfCom,&tDoid);
    }
    LOG_RUN(en_orb_inited,"Orb initialized.localService Id:%u, skeleton Nr:%d,",(TUInt32)m_nLocalServiceId,nSkeletonNr);
    return SUCCESS;
}

void COrb::StopService(TUInt32 nServiceID)
{
    if (0xFFFFFFFF != nServiceID)
    {
        m_nService2Stop = nServiceID;
        while (m_nService2Stop == nServiceID) //改为-1
        {
            SleepT(15); //不停的尝试.
        }
        LOG_RUN(en_stop_service,"Service %d Stoped!",nServiceID);
    }
}


TInt32 COrb::RegisterRun(IfSkeleton *pIfSkeleton,TUInt32 nGapInMs)
{
    CListNode<CArrayPoolNode<CSkeleton> > *pSk = (CListNode<CArrayPoolNode<CSkeleton> >*)pIfSkeleton;
    if (pSk->m_pListBelongsTo)
    {
        pSk->m_pListBelongsTo->Detach(pSk);
        pSk->m_pListBelongsTo = NULL;
    }
    m_tRunning.push_back(pSk);
    pSk->m_pListBelongsTo = &m_tRunning;
    CDoid *pDoid = pSk->GetMyDoid();
#ifdef _DEBUG
    if (pDoid)
    {
#endif
        LOG_DEBUG(en_obj_registered_run,"Obj [%d,%d] registered run.",(TUInt32)pDoid->m_srvId,(TUInt32)pDoid->m_objId);
#ifdef _DEBUG
    }
#endif
    return SUCCESS;
}
    //时间相关
const CClock *COrb::GetClock()
{
    return m_pClock;
}


TInt32 COrb::Run(const TInt32 threadId,const TInt32 runCnt)
{
    int nUsedCnt = 0;
    TInt32 nGap = m_pClock->GetTimeGap(m_nLastCheckTime);
    if (nGap >= 40)
    {
        m_nLastCheckTime = m_pClock->GetLocalTime();
        CListNode<CArrayPoolNode<CSkeleton> >  *pHeader = m_tRunning.header();
        CListNode<CArrayPoolNode<CSkeleton> >  *pRear = m_tRunning.header();
        while (pHeader)
        {
            nUsedCnt += pHeader->OnRoutine(runCnt);
            pHeader = pHeader->GetNext();
        }
    }
    CMessageHeader *pMsg = m_pIfComm->GetMsg();
    while (pMsg)
    {
        CDoid *pDest = pMsg->GetDestDoidByIdx();
        CListNode<CArrayPoolNode<CSkeleton> > *pSk = m_tSkeletonPool.FindMem(pDest->m_objId);
        if (pSk->m_pListBelongsTo != &m_tSkeletonPool.m_tFree)
        {
            if (pSk->m_tDoid == *pDest)
            {
                pSk->OnRecv(pMsg);
            }
        }
        for (int i=1;i<=pMsg->GetBroadcastDoidNr();++i)
        {
            pDest = pMsg->GetDestDoidByIdx(i);
            CListNode<CArrayPoolNode<CSkeleton> > *pSk = m_tSkeletonPool.FindMem(pDest->m_objId);
            if (pSk->m_pListBelongsTo != &m_tSkeletonPool.m_tFree)
            {
                if (pSk->m_tDoid == *pDest)
                {
                    pSk->OnRecv(pMsg);
                }
            }
        }
        ++nUsedCnt;
        m_pIfComm->ReturnMsgBuff(pMsg);
        if (nUsedCnt > runCnt)
        {
            break;
        }
        else
        {
            pMsg = m_pIfComm->GetMsg();
        }
    }
    CConnectionEvent tEvent;
    TInt32 nRet = m_pIfComm->GetNetEvent(tEvent);
    while (SUCCESS <= nRet) //net Event 必须全处理了
    {
        for (TInt32 n=0;n<MAX_SERVICE_NR;++n)
        {
            if (m_ppServiceSkeleton[n])
            {
                m_ppServiceSkeleton[n]->OnNetEvent(&tEvent);
            }
        }
        nRet = m_pIfComm->GetNetEvent(tEvent);
        ++nUsedCnt;
    }
    return nUsedCnt;
}

IfCommunicator *COrb::GetCommunicator()
{
    return m_pIfComm;
}

}
#include "../include/Timer.h"


namespace Zephyr
{

TInt32 CTimer::Init(TInt32 nSize)
{
    TInt32 nRet = m_tPool.InitPool(nSize);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    m_tMap.Init(&m_tPool);
    m_pRuning = NULL;
    return SUCCESS;
}

void *CTimer::SetTimer(TUInt32 uGapInMs,TInt32 nRepeatTime,IfScheduler *pScheduler,TUInt64 nTimeNow)
{
    TplMultiKeyMapNode<CScheduler,TUInt64> *pNode = m_tPool.GetMem();
    if (pNode)
    {
        pNode->m_nTime = nTimeNow + uGapInMs;
        pNode->m_nGap  = uGapInMs;
        pNode->m_nRepeatTime = nRepeatTime;
        pNode->m_pScheduler = pScheduler;
        m_tMap.AddInTree(pNode);
    }
    return pNode;
}

    //删除定时器pTimer是SetTimer返回的结果
IfScheduler *CTimer::KillTimer(void *pTimer)
{
    TplMultiKeyMapNode<CScheduler,TUInt64> *pNode = (TplMultiKeyMapNode<CScheduler,TUInt64> *)pTimer;
    if (pNode == m_pRuning) //正是当前节点，自己删自己！
    {
        IfScheduler *pScheduler = pNode->m_pScheduler;
        pNode->m_nRepeatTime = 0;
    }
    else
    {
        m_tMap.RemoveFromTreeItem(pNode);
        IfScheduler *pScheduler = pNode->m_pScheduler;
        pNode->Clean();
        m_tPool.ReleaseMem(pNode);
        return pScheduler;
    }
}

    //Run是要定时去调用的，然后产生回调.
TUInt32 CTimer::Run(TUInt64 nTimeNow)
{
    TUInt32 nRunCnt = 0;
    //if (m_tMap.GetActivedSize())
    {
        TplMultiKeyMapNode<CScheduler,TUInt64> *pNode = m_tMap.First();
        while(pNode)
        {
            if (pNode->m_nTime < nTimeNow)
            {
                m_pRuning = pNode; //保证m_pRuning不会被删除.
                pNode->m_pScheduler->OnScheduler(pNode,nTimeNow);
                m_pRuning = NULL;
                //if (pNode->m_pScheduler)
                {
                    m_tMap.RemoveFromTreeItem(pNode); //先从tree切掉自己
                    if (pNode->m_nRepeatTime)
                    {
                        if (pNode->m_nRepeatTime > 0)
                        {
                            --pNode->m_nRepeatTime;
                        }
                        pNode->m_nTime += pNode->m_nGap;
                        m_tMap.AddInTree(pNode);
                    }
                    else
                    {
                        pNode->Clean();
                        m_tPool.ReleaseMem(pNode);
                    }
                }
                pNode = m_tMap.First();//这样做效率不高Log(n)，但是最安全，限制最少.
                ++ nRunCnt;
                if (nRunCnt > 10000)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    return nRunCnt;
}


}
/*-------------------------------------------------------------
 Copyright (C)| 
 File: Timer.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ��ʱ��
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_PUBLIC_TIMER_H__
#define __ZEPHYR_PUBLIC_TIMER_H__
#include "../Interface/Platform/include/IfTimer.h"
#include "../Interface/App/include/IfScheduler.h"
#include "../tpl/include/TplMultiMap.h"
#include "../tpl/include/TplPool.h"
namespace Zephyr
{


class CScheduler
{
public:
    CScheduler()
    {
        Clean();
    }

    TUInt64 &GetKey()
    {
        return m_nTime;
    }

    TUInt64 m_nTime;
    IfScheduler *m_pScheduler;
    TUInt32     m_nGap;
    TInt32     m_nRepeatTime;
    void Clean()
    {
        m_nTime = 0;
        m_pScheduler = 0;
        m_nGap = 0;
        m_nRepeatTime = 0;
    }
};

class CTimer : public IfTimer
{
private:
    CPool<TplMultiKeyMapNode<CScheduler,TUInt64> > m_tPool;
    TplMultiKeyMap<CScheduler,TUInt64>             m_tMap;
    TplMultiKeyMapNode<CScheduler,TUInt64>*        m_pRuning;//��ǰ����run�Ľڵ㣬����ɾ��
public:
    TInt32 Init(TInt32 nSize);

    void *SetTimer(TUInt32 uGapInMs,TInt32 nRepeatTime,IfScheduler *pScheduler,TUInt64 nTimeNow);

    //ɾ����ʱ��pTimer��SetTimer���صĽ��
    IfScheduler *KillTimer(void *pTimer);

    //Run��Ҫ��ʱȥ���õģ�Ȼ������ص�.
    TUInt32 Run(TUInt64 nTimeNow);

};

}


#endif

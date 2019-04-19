/*-------------------------------------------------------------
 Copyright (C)| 
 File: Orb.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: Orb层，使用一个IfComm
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_SERVICE_CONTAINER_ORB_H__
#define __ZEPHYR_SERVICE_CONTAINER_ORB_H__

#include "../../include/TypeDef.h"
#include "../../include/Clock.h"
#include "../../Interface/Platform/include/IfOrb.h"
#include "../../Interface/Platform/include/IfTask.h"
#include "../../Interface/Platform/include/IfCommunicator.h"
#include "../../tpl/include/TplPool.h"
#include "../../Interface/Platform/include/IfLogger.h"
#include "Skeleton.h"

#define MAX_SERVICE_NR 64

namespace Zephyr
{

class COrb : public IfOrb , public IfTask
{
private:
    IfCommunicator *m_pIfComm;
    //IfObj          *m_pRunObj;
    TUInt32         m_nRunGapInMs;
    TUInt32         m_nLastCheckTime;
    TUInt16         m_nLocalNodeId;
    TUInt16         m_nLocalVIP;
    //这个Orb的service从这个开始
    TUInt16         m_nLocalServiceId;

    TUInt16         m_nLocalServiceIdEnd;
    //按16位来循环使用objId
    TplArrayPool<CSkeleton>    m_tSkeletonPool;

    const CClock * m_pClock;
    CSkeleton       *m_ppServiceSkeleton[MAX_SERVICE_NR];

    //需要run的都放这里,40ms跑一次
    TUInt32         m_nLastRunTime;
    CList<CArrayPoolNode<CSkeleton> > m_tRunning;

    IfLogger        *m_pLogger;


             
    volatile TUInt32    m_nService2Stop;
public:
    COrb();
    ~COrb();
    void StopService(TUInt32 nServiceID);
    TInt32 Init(IfCommunicator *pIfCom,CDoid *pDoidBegin,TInt32 nSkeletonNr,IfLogger *pLogger);
    
    //注册特定的Srv
    virtual IfSkeleton* RegisterObj(IfObj *pObjSkeleton,TInt16 nSrvId);

    IfSkeleton* RegiterService(IfObj *pObj,TInt16 nSrvId);
    //注销
    virtual void    UnRegisterObj(IfSkeleton *pStub);

    //时间只能是1ms、10ms、100ms、1s四种
    virtual TInt32 RegisterRun(IfSkeleton *pObj,TUInt32 nGapInMs);
    //时间相关
    virtual const CClock *GetClock();

    virtual IfCommunicator *GetCommunicator();

public:
    virtual TInt32 Begin(TInt32 threadId)
    {
        return SUCCESS;
    }
    virtual TInt32 Run(const TInt32 threadId,const TInt32 runCnt);
    virtual TInt32 End(TInt32 threadId)
    {
        return SUCCESS;
    }
public:
    IfLogger *GetLogger()
    {
        return m_pLogger;
    }
};

}

#endif

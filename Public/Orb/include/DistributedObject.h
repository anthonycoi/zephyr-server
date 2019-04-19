/*-------------------------------------------------------------
 Copyright (C)| 
 File: DistributedObject.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 一个分布式对象
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_SERVICE_CONTAINER_DISTRIBUTED_OBJECT_H__
#define __ZEPHYR_SERVICE_CONTAINER_DISTRIBUTED_OBJECT_H__

#include "../../include/TypeDef.h"
#include "../../include/SysMacros.h"
#include "../../Interface/App/include/IfObj.h"
#include "../../Interface/Platform/include/IfCommunicator.h"
#include "../../Interface/Platform/include/IfSkeleton.h"
namespace Zephyr
{

class CObject : public IfObj
{
protected:
    IfSkeleton *m_pSkeleton;
    CMessageHeader *m_pCurrentMsg;
    //IfCommunicator  *m_pIfComm;

    TInt64      m_nMsgHandled;
    TInt64      m_nMsgSent;
    TInt64      m_nMsgDroped;
    TInt64      m_nStartTime;
public:
    //只能由Service\ServiceContainer去注册.
    void   SetSkeleton(IfSkeleton *pSkeleton)
    {
        m_pSkeleton = pSkeleton;
    }
    
    CObject();
    TInt32 Init();
    //在初始化的时候会被调.
    virtual TInt32      OnInit();
    //结束是回调.
    virtual TInt32      OnFinal();

    CDoid *GetMyDoid();
    //获得后自己初始化
    CMessageHeader *PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid* destDoid,TInt32 destDoidNum,bool bRearrangeDest);
    //发送消息
    TInt32  SendMsg(CMessageHeader *pMsg);

    /*virtual TInt32  OnRecv(CMessageHeader *pMsg); //这些函数如果*/
    virtual TInt32 OnRecvUnacceptableMsg(CMessageHeader *pMsg)
    {
        //drop this msg;
        return SUCCESS;
    }
//     //定时  器到时了
//     virtual TInt32  OnTimer(TInt32 nTimerIdx,void *pData,TInt32 nTimeGap,TUInt32 nTimeNow)
//     {
//         return 0;
//     }

    //以下是给Service使用的
    //定期的回调，可以注册循环时间，但只能有一个
    virtual TInt32  OnRoutine(TUInt32 nRunCnt)
    {
        return 0;
    }
    //网络时间
    virtual TInt32  OnNetEvent(CConnectionEvent *pEvent)
    {
        return SUCCESS;
    }

    //这个更多的
    CDoid *GetCallerDoid()
    {
        if (m_pCurrentMsg)
        {
            return m_pCurrentMsg->GetSrcDoid();
        }
        //如果不是远程调用，那就是本地调用，调用者就是自己.
        return GetMyDoid(); 
    }

    IfSkeleton *GetSkeleton()
    {
        return m_pSkeleton;
    }
};

}



#endif

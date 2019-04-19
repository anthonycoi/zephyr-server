/*-------------------------------------------------------------
 Copyright (C)| 
 File: NetTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  网络层工人，每人维护一个completionPort,维护一个connection列表然后不停的收发数据
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_NET_TASK_H__
#define __ZEPHYR_NET_TASK_H__
//#include "TplMap.h"
#include "IfTask.h"
#include "Connection.h"
#include "ConnectionPool.h"
namespace Zephyr
{




class CConnectionMgr;

//只负责收发消息，发现网络断开，通知一下，其它都不做
//如果网络断开后，则后续所有操作失败。
class CNetTask : public IfTask
{
protected:
    HANDLE  m_completionPort;
    //TplMap<CConnection,TUInt32>                 m_connectionPool;
    //TplNode<CNetEvent,TUInt16>           *m_pWorkerEventPool[NR_OF_QUEUE_IN_TASK];

    //events that net task generated.
    //TplNode<CNetEvent,TUInt16> *m_pNetTaskEvent[NR_OF_QUEUE_IN_TASK];
    ////events that app generated.
    //TplNode<CNetEvent,TUInt16> *m_pAppEvent[NR_OF_QUEUE_IN_TASK];
    
    
    ItemClassPool<CConnection>*     m_pConnectionPool;         
    
    CNetEventQueues*     m_pEventQueues;
    
    volatile  TUInt32    m_connectionAborted;

#ifdef _DEBUG
    TUInt32                                     m_startTime;
    TUInt32                                     m_runCnt;
    TUInt32                                     m_averageTime;
#endif
    TUInt32                                     m_buffSize;
    TUChar*                                     m_pBuff;
public:
    CNetTask();
    ~CNetTask();
    TInt32         Init(HANDLE completionPort,ItemClassPool<CConnection> *pPool,CNetEventQueues *pQueues,TInt32 maxMsgLen);
    void           Final();
    HANDLE GetCompletionPort()
    {
        return m_completionPort;
    }
    TInt32 GetAbortedConnectionNr()
    {
        return (m_connectionAborted);
    }

    virtual TInt32 Begin(TInt32 threadId);
    virtual TInt32 Run(const TInt32 threadId,const TInt32 runCnt);
    virtual TInt32 End(TInt32 threadId);
private:
    
    //void   CloseConnection(CConnection *pConnection);

    //put the connection in to app
    //TInt32 OnAppSend(TplNode<CNetEvent,TUInt16>* pConnection);
    
    //inline TInt32 AddNetEvent(CConnection *pConnection,TUInt16 eventType);
    TInt32 ProcessIOMessage(CIocpOverlappedDataHeader *pHeader,TUInt32 ioSize);
    //TInt32 Send(CIocpOverlappedDataHeader *pHeader);
    //TInt32 Recv(CIocpOverlappedDataHeader *pHeader);
    
};


}



#endif

 

/*-------------------------------------------------------------
 Copyright (C)| 
 File: ConnectionMgr.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 连接层，标示一个连接管理者
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/



#ifndef __ZEPHYR_CONNECTION_MGR_H__
#define __ZEPHYR_CONNECTION_MGR_H__

#include "TypeDef.h"
#include "SysMacros.h"
#include "connection.h"
#include "IfNet.h"
#include "IfNetApp.h"
#include "IfTask.h"
#include "IfTaskMgr.h"
#include "NetTask.h"
#include "Listener.h"
#include "IfParser.h"
#include "Connector.h"
#include "NetEventQueue.h"
#include "..\..\System\include\ProducerAndConsumer.h"
#include <stdio.h>
namespace Zephyr
{


class CConnectionMgr : public IfNet
{
protected:
    CListener*          m_pListeners;
    TUInt16             m_nrOfListener;

    HANDLE              m_hCompletionPort;
    
    CConnector          m_connector;
    //一个worker
    CNetTask            m_netWorker;
    ItemClassPool<CConnection>     m_conncectionPool;
    
    IfParserFactory*    m_pParserFactory;
    IfCryptorFactory*   m_pCryptorFactory;
    
    CNetEventQueues     m_netEventQueues;

    TUInt32             m_nTimeNow;

//         
//         //TUInt32           m_lastRunTo;
//         //网络层事件
//         CIoEventQueue       m_netEvents;
//         //应用层事件
//         CIoEventQueue       m_appEvents;
private:
    TUInt32                                     m_buffSize;
    TUChar*                                     m_pBuff;

public:
    //主动连接
    virtual TInt32 Connect(const TChar *pRemoteIp,const TChar *pMyIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack);
    //virtual TInt32 Connect(TUInt32 remoteIp,TUInt32 myIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack) = 0;

    virtual TInt32 Connect(const TUInt32 remoteIp,const TUInt32 myIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack);
    
    virtual void *Listen(const TChar *pIp,TUInt16 port,TUInt16 maxConnection,void *pIfCallBack);

    virtual void *Listen(TUInt32 nIp,TUInt16 nPort,TUInt16 nMaxConnection,void *pIfCallBack);

    virtual TInt32 StopListening(void *pListeningPointer);


    //if chOpt = 1,disable the Nagle algrithom.
    //else enable it , it is enableed by default.
    //In general this is unnecessary.
    //产生connection回调.
    virtual TInt32 Run(TUInt32 runCnt);
    
public:
    CConnectionMgr();
    ~CConnectionMgr();

    TInt32 Init(TUInt32 maxConnectionNum,IfTaskMgr *pTaskMgr,IfParserFactory* pParserFactory,IfCryptorFactory *pIfCryptorfactory,TUInt32 nSendBuffSize,TUInt32 nRecvBuffSize);
    void   Final();
    void   TestReference(IfTask& task)
    {
        task.Begin(1);
    }
};



}

#endif
 
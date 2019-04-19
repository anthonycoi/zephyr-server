/*-------------------------------------------------------------
 Copyright (C)| 
 File: Connection.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 连接层，标示一个连接
 Others:  算法是这样的，所有连接创建都在app层，app管理lilsten、connect、disconnect.
          network 从connectionMgr获取connectionEvent,只有两个，那就是
          1)event_connection_is_established  说明app层才创建好连接，但是没有开始监听事件，这个时候net层要开始监听事件
          2)event_connection_has_new_data_to_send 说明app层刚刚发送了一个数据到net层的pipe，net要负责发送数据
          app从connectionMgr获取connectionEvent, 也只有两个，那就是：
          1)event_connection_has_new_data_to_read 说明网络层刚刚收一些新数据，需要应用层来处理
          2)event_connection_is_broken 说明网络层发现连接中断了，报告app层
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/



 #ifndef __ZEPHYR_COMM_NET_CONNECTION_H__
 #define __ZEPHYR_COMM_NET_CONNECTION_H__

#include "TypeDef.h"

#include "SysMacros.h"

#include "Pipe.h"

#include "IocpOverlappedDataHeader.h"

#include "IfNetData.h"
#include "Lock.h"
#include "IfNet.h"
#include "IfNetApp.h"
#include "IfParser.h"
#include "IfCryptor.h"
#include "NetEventQueue.h"
namespace Zephyr
{


enum EnConncectionType
{
    connection_is_passive               = 0,
    connection_is_postive               = 1,
};

enum EnConnectionEvent
{
    event_connection_nothing                 = 0,
    event_connection_is_established             ,
    event_connection_has_new_data_to_send       ,
    event_connection_has_new_data_to_read       ,
    event_connection_is_broken                  ,
    event_connection_app_handled                ,
    event_connection_is_aborted                 ,
};

class CConnectionMgr;


//只管收发，并不监听、产生任何事件，所有监听、产生事件由其它类负责.
//断开流程:CConnection只负责关闭socket,不会主动关闭socket
//告诉CNetWork，连接是否可以关闭.
//主动关闭流程:应用端发送一个IOEvent给Net,告诉Net关闭连接，net收到后把所有数据发送后，关闭连接.并通过IOEvent通知ConnectionMgr，让ConnectionMgr回收Connection
//被动关闭流程:socket断开后，net直接closesocket，然后通过IOEvent通知CConnectionMgr.
//状态变化为    connection_is_not_in_use            = 0x00, 由应用层线程切换至connection_is_trying->connection_is_using
//由网络层线程将其->connection_is_broken、或者connection_is_aborted
//最后由应用层线程来释放->connection_is_not_in_use
class CConnection : public IfConnection
{
private:
    //Record the file Handle.
    CConPair                    m_key;
    SOCKET                      m_socket;
    
    TInt16                      m_connectionIdx;
    TInt16	                    m_errorCode;
    TUInt32                     m_seqNum;
    
    TUInt32                      m_maxHeaderLength;
    TUInt32                      m_minHeaderLength;
    
    volatile TUChar             m_connectionState;
    volatile TUChar             m_netDisconnect;
    volatile TUChar             m_appDisconnect;
    volatile TUChar             m_connectionType;

    volatile TUInt32            m_nNetBlocked;

    TUInt32                     m_uLastNetAppBlocked;
    TUInt32                     m_uLastAppAppBlocked;

    TUInt16                     m_uNetBlockedTime;
    TUInt16                     m_uAppBlockedTime;

    CPipe                       m_inPipe;
    CPipe                       m_outPipe;
    
    IfConnectionCallBack*       m_pAppCallBack;
    IfParser*                   m_pIfParser;
    IfCryptor*                  m_pIfCryptor;
    CNetEventQueues*            m_pEventQueues;
    //TUChar                      *m_pConfirmedTo;
    //只有一个工作者，不需要锁，即使是1000M网卡，IO最多128*2 M，256M数据，够了！
    //CLock                                       m_wLock;
    //CLock                                       m_rLock;
    #ifdef _DEBUG
    TUInt64                     m_msgRecv;
    TUInt64                     m_msgSend;


    #endif
    //TUChar m_pRecvBuff[1024*4];
    #ifdef _WIN32
    CIocpOverlappedDataHeader   m_readDataHeader;
    CIocpOverlappedDataHeader   m_writeDataHeader;
    #endif
    //应用层发送序列号
    volatile TUInt32            m_appSeqNum;
    //网络层确认序列号
    volatile TUInt32            m_netConfirmNum;
    
    //网络层发送序列号
    volatile TUInt32            m_netSeqNum;
    //应用层确认序列号
    volatile TUInt32            m_appConfirmNum; 

    TUInt32*                    m_pTimer;
    DECLARE_CLASS_LIST(CConnection)
public:
    virtual TInt32 SendMsg(TUChar *pMsg,TUInt32 msgLen);
    //应用层调用这个完毕后，网络层会在尝试发送完所有消息后关闭连接，并且丢弃所有收到的消息.
    virtual TUInt32 GetFreeBuffLength();
    virtual TInt32 Disconnect();
    virtual CConPair *GetConnectionInfo();
    virtual TInt32 NeedNoDelay(const char chOpt) ;
    virtual TInt32 GetPendingDataLen();
    virtual EnConnectionState GetConnctionState();
public:
    CConnection();
    void SetTimer(TUInt32 *pTimer)
    {
        m_pTimer = pTimer;
    }
    TInt32  Init()
    {
        return SUCCESS;
    }
    void    Final();
    TInt32 OnInit();
    void SetEventQueue(CNetEventQueues *pEventQueue)
    {
        m_pEventQueues = pEventQueue;
    }
//     TUInt32 GetEvent()
//     {
//         if (m_connectionState == connection_is_not_in_use)
//         {
//             return event_connection_nothing;
//         }
//         TUChar *pNewTo = m_inPipe.GetDataHeader();
//         if (pNewTo != m_pConfirmedTo)
//         {
//             //m_pConfirmedTo = pNewTo;
//             return event_connection_has_new_data;
//         }
//         if (m_connectionState == connection_is_broken)
//         {
//             return event_connection_broken;
//         }
//         return event_connection_nothing;
//     }
    
    TBOOL IsConnected()
    {
        if (connection_is_using == m_connectionState)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsActived()
    {
        if (m_connectionState == connection_is_not_in_use)
        {
            return false;
        }
        return true;
    }
   

    
    TInt32 GetConnectionState()
    {
        return m_connectionState;
    }
    void SetConnectionType(EnConncectionType t)
    {
        m_connectionType = t;
    }
    TInt32 GetConnectionType()
    {
        return m_connectionType;
    }

    TUInt16 GetConnectionIdx()
    {
        return m_connectionIdx;
    }
    TUInt32 GetSeqNum()
    {
        return m_seqNum;
    }
    
    TInt32 SetSocketOnAccept(SOCKET socket,CConnectionMgr *pMgr);
    
    SOCKET GetSocket()
    {
        return m_socket;
    }
    
    TInt32 OnCreate(TUInt32 idx,TUInt32 nSendPipeSize,TUInt32 nRecvPipeSize);
    void OnTryConnecting()
    {
        m_connectionState = connection_is_trying;
    }
    
    //应用层完成了socket连接，由应用层线程调用
    TInt32 OnConnected();
    //连接中断了，由应用层调用
    TInt32 OnDisconnected();
    
    
    
    
    
    //应用层发数据了，由应用层线程调用
    void OnAppSend();
    //由应用层线程调用
    void OnAppDisconnected();
    //应用层收取了网络层的数据，由应用层线程调用
    void OnAppRecved();
    
    void OnAppHandled();

    void OnNetContinue();
    
    //网络层发送了数据，由网络层线程调用
    void OnNetSent();
    //网络层收到了数据，由网络层线程调用
    void OnNetRecv();
    //连接中断，由网络层线程调用
    void OnNetDisconnected();
    
    
    TBOOL CheckReadHeader(CIocpOverlappedDataHeader *pHeader)
    {
        if (pHeader == &m_readDataHeader)
        {
            if (m_readDataHeader.m_seqNum == m_seqNum)
            {
                return TRUE;
            }
        }
        return FALSE;
    }

    TBOOL CheckWriteHeader(CIocpOverlappedDataHeader *pHeader)
    {
        if (pHeader == &m_writeDataHeader)
        {
            if (m_writeDataHeader.m_seqNum == m_seqNum)
            {
                return TRUE;
            }
        }
        return FALSE;
    }
    
    //TInt32 SetSocketOptions();
    
    TInt32 OnFinal();
    
    TInt32 Init(SOCKET socket,CConPair *pPair,void *pAppCallBack,void *pParser,void* pCryptor=NULL);
    
    TInt32 GetAvailableReadLength()
    {
        return m_inPipe.GetDataLen();
    }
    //TUChar *Peek(TUChar *pData,TUInt32 needLength);
    
    TInt32 OnRecv(CIocpOverlappedDataHeader *pHeader,TUInt32 ioSize);
    TInt32 OnSent(CIocpOverlappedDataHeader *pWriteDataHeader,TUInt32 ioSize);
    CIocpOverlappedDataHeader *GetWritingData();
	CIocpOverlappedDataHeader *PrepareToRead();
	
	//由网络层调用
	//如果返回-1,则表示连接中断,，始终保持有一个Read CIocpOverlappedDataHeader在完成队列中,
	//根据GetQueuedCompletionStatus 来判断结束.应用层始终通知应用层这个事件
	TInt32 OnWriteEvent();
	//由应用层调用
	TInt32 OnReadEvent();
    
    //disconnect the connection
	void CloseConnection();
	
	
    //由应用层调用，如果返回-1，则表示需要释放连接,把链接close,并且放回connectionPool
    TInt32 AppRoutine(TUChar *pBuff,TUInt32 buffLen);
    //由NetTask掉也难怪，如果返回-1，
    TInt32 NetRoutine();
    
    
public:
    TInt32 DisableNagle(const char chOpt);
    
private:
    TInt32 Reconnect();
    //TInt32 CloseConnection();
    void CheckAppDisconnected();
    inline TInt32 GetNetWaitTime();
    inline TInt32 GetAppWaitTime();
    inline TUInt32 GetTimeNow();
};

}

 #endif
 

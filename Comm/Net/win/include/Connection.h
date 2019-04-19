/*-------------------------------------------------------------
 Copyright (C)| 
 File: Connection.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ���Ӳ㣬��ʾһ������
 Others:  �㷨�������ģ��������Ӵ�������app�㣬app����lilsten��connect��disconnect.
          network ��connectionMgr��ȡconnectionEvent,ֻ���������Ǿ���
          1)event_connection_is_established  ˵��app��Ŵ��������ӣ�����û�п�ʼ�����¼������ʱ��net��Ҫ��ʼ�����¼�
          2)event_connection_has_new_data_to_send ˵��app��ոշ�����һ�����ݵ�net���pipe��netҪ����������
          app��connectionMgr��ȡconnectionEvent, Ҳֻ���������Ǿ��ǣ�
          1)event_connection_has_new_data_to_read ˵�������ո���һЩ�����ݣ���ҪӦ�ò�������
          2)event_connection_is_broken ˵������㷢�������ж��ˣ�����app��
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


//ֻ���շ������������������κ��¼������м����������¼��������ฺ��.
//�Ͽ�����:CConnectionֻ����ر�socket,���������ر�socket
//����CNetWork�������Ƿ���Թر�.
//�����ر�����:Ӧ�ö˷���һ��IOEvent��Net,����Net�ر����ӣ�net�յ�����������ݷ��ͺ󣬹ر�����.��ͨ��IOEvent֪ͨConnectionMgr����ConnectionMgr����Connection
//�����ر�����:socket�Ͽ���netֱ��closesocket��Ȼ��ͨ��IOEvent֪ͨCConnectionMgr.
//״̬�仯Ϊ    connection_is_not_in_use            = 0x00, ��Ӧ�ò��߳��л���connection_is_trying->connection_is_using
//��������߳̽���->connection_is_broken������connection_is_aborted
//�����Ӧ�ò��߳����ͷ�->connection_is_not_in_use
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
    //ֻ��һ�������ߣ�����Ҫ������ʹ��1000M������IO���128*2 M��256M���ݣ����ˣ�
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
    //Ӧ�ò㷢�����к�
    volatile TUInt32            m_appSeqNum;
    //�����ȷ�����к�
    volatile TUInt32            m_netConfirmNum;
    
    //����㷢�����к�
    volatile TUInt32            m_netSeqNum;
    //Ӧ�ò�ȷ�����к�
    volatile TUInt32            m_appConfirmNum; 

    TUInt32*                    m_pTimer;
    DECLARE_CLASS_LIST(CConnection)
public:
    virtual TInt32 SendMsg(TUChar *pMsg,TUInt32 msgLen);
    //Ӧ�ò���������Ϻ��������ڳ��Է�����������Ϣ��ر����ӣ����Ҷ��������յ�����Ϣ.
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
    
    //Ӧ�ò������socket���ӣ���Ӧ�ò��̵߳���
    TInt32 OnConnected();
    //�����ж��ˣ���Ӧ�ò����
    TInt32 OnDisconnected();
    
    
    
    
    
    //Ӧ�ò㷢�����ˣ���Ӧ�ò��̵߳���
    void OnAppSend();
    //��Ӧ�ò��̵߳���
    void OnAppDisconnected();
    //Ӧ�ò���ȡ�����������ݣ���Ӧ�ò��̵߳���
    void OnAppRecved();
    
    void OnAppHandled();

    void OnNetContinue();
    
    //����㷢�������ݣ���������̵߳���
    void OnNetSent();
    //������յ������ݣ���������̵߳���
    void OnNetRecv();
    //�����жϣ���������̵߳���
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
	
	//����������
	//�������-1,���ʾ�����ж�,��ʼ�ձ�����һ��Read CIocpOverlappedDataHeader����ɶ�����,
	//����GetQueuedCompletionStatus ���жϽ���.Ӧ�ò�ʼ��֪ͨӦ�ò�����¼�
	TInt32 OnWriteEvent();
	//��Ӧ�ò����
	TInt32 OnReadEvent();
    
    //disconnect the connection
	void CloseConnection();
	
	
    //��Ӧ�ò���ã��������-1�����ʾ��Ҫ�ͷ�����,������close,���ҷŻ�connectionPool
    TInt32 AppRoutine(TUChar *pBuff,TUInt32 buffLen);
    //��NetTask��Ҳ�ѹ֣��������-1��
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
 

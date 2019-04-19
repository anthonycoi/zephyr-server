#include "AppConnection.h"
#include "stdio.h"
#include "stdlib.h"
#include <windows.h>
#include "AppConnectionMgr.h"
#include <iostream> 
#include <time.h> 
using namespace Zephyr;


int CAppConnection::sm_averageMsgLen = 0;

CAppConnection::CAppConnection()
{
    m_pIfConnection = NULL;
    m_msgRecved = 0;
    m_msgSend = 0;
    m_passiveSendNr = 0;
    m_pNext = NULL;
    m_actived = 0;
}



TInt32 CAppConnection::OnInit()
{
    m_msgRecved = 0;
    m_msgSend = 0;
    m_passiveSendNr = 0;
    m_pIfConnection = NULL;
    m_pNext = NULL;
    m_actived = 1;
    m_connectedTime = 0;
    m_lastLogTime = timeGetTime();
    //�����������һ��bug����Ϊת�ĺܿ죬time(NULL)������ֵͬ�����������rand()������ֵͬ
    //srand(time(NULL));
    m_unconnectedTime = (rand()%10000);
    return SUCCESS;
}
TInt32 CAppConnection::OnFinal()
{
//     m_msgRecved = 0;
//     m_msgSend = 0;
//     m_passiveSendNr = 0;
//     m_pIfConnection = NULL;
    m_actived = 0;
    return SUCCESS;
}


TInt32 CAppConnection::Run()
{
    if (!m_actived)
    {
        return 0;
    }
    //if (m_passiveSendNr)
    {
        unsigned int timeNow = timeGetTime();
        m_connectedTime += (timeNow - m_lastLogTime);
        m_lastLogTime = timeNow;
        
        if (m_connectedTime > (m_unconnectedTime + 5000))
        {
            m_connectedTime = 0;
            m_unconnectedTime= 0;
            switch(m_actived)
            {
            case 1:
                {
                    //return 0;

                }
                break;
            case 2:
                {
                    if (m_pIfConnection)
                    {
//                         m_pIfConnection->Disconnect();
//                         m_msgRecved = 0;
//                         m_msgSend = 0;
//                         if (!m_passiveSendNr)
//                         {
//                              OnDissconneted(-1);
//                         }
//                         //printf("[CAppConnection::Run]App disconnected!");
//                         m_pIfConnection = NULL;
//                         m_actived = 3;
                        return 0;
                    }
                }
                break;
            case 3:
                {
                    if (m_passiveSendNr)
                    {
                        m_actived = 1;
                        OnInit();
                        m_passiveSendNr = 100;
                        return -5;
                    }
                }
                break;
            }
        }
    }
    long long len = m_msgRecved + m_passiveSendNr;
    if (len > m_msgSend)
    {
        len -= m_msgSend;
    }
    else
    {
        return 0;
    }
    if (len <= 0)
    {
        return 0;
    }
    
    if (m_pIfConnection && sm_averageMsgLen)
    {
        static int buff[32*1024];
        int sendLen = 100;
//         sendLen <<= 1;
//         sendLen &= 0xFFFFFFFC;
        
        for (int i=0;i<(sendLen>>2);++i)
        {
            buff[i] = sendLen+i;
        }
        
        int ret = m_pIfConnection->SendMsg((TUChar *)buff,sendLen);
        if (SUCCESS < ret)
        {
            m_msgSend += sendLen;
        }
        return sendLen;
    }
    return SUCCESS;
}

TInt32 CAppConnection::OnRecv(TUChar *pMsg, TUInt32 msgLen)
{
    #ifdef __PRINT_DEBUG_INFO__
    printf("[CAppConnection::OnRecv]");
    #endif
    TUInt32 *pLen = (TUInt32*)pMsg;
    if (*pLen != msgLen)
    {
        printf("Wrong size!");
        return msgLen;
    }
    TUInt32 begin = *pLen;
    for (int i=0;i<(msgLen>>2);++i)
    {
        if (*pLen != (begin + i))
        {
            printf("Wrong Size!");
            return msgLen;
        }
        ++pLen;
    }
    m_msgRecved += msgLen;
    Run();
    return SUCCESS;
}
    //virtual TInt32 OnRecvIn2Piece(TUChar *pMsg, TUInt32 msgLen,TUChar *pMsg2,TUInt32 msgLen2) = 0;
    //�������Զ���factory����parser��crypter,��Ӧ�ò��������������������
    //Ӧ�ò�Ӧ����ȷ֪��IfParser �� IfCryptor��ʵ���࣬����OnConnected��ʱ������������.
    //�������ֵen_if_connection_callback_on_connected_release_parser �����ˣ���ôparser�ᱻ�ͷ�
    //�������ֵen_if_connection_callback_on_connected_release_cryptor �����ˣ���ôpCryptor�ᱻ�ͷ�
    //IfConnection *pIfConnection������ʵ�ʽ�����ʱ���ٴ���Ӧ�ò㡣
TInt32 CAppConnection::OnConnected(IfConnection *pIfConnection,IfParser *pParser,IfCryptor *pCryptor)
{
//#ifdef __PRINT_DEBUG_INFO__
    //printf("[CAppConnection::OnConnected]");
//#endif
    m_pIfConnection = pIfConnection;
    m_actived = 2;
    m_msgRecved = 0;
    m_msgSend = 0;
    return SUCCESS;
}

    //�κ�socket�쳣�����Զ��ر���������
TInt32 CAppConnection::OnDissconneted(TInt32 erroCode)
{
//#ifdef __PRINT_DEBUG_INFO__
    //printf("[CAppConnection::OnDissconneted]");
//#endif
    if (!m_passiveSendNr)
    {
        m_pConnectionMgr->ReleaseConnection(this);
    }
    m_actived = 3;
    m_pIfConnection = NULL;
    return SUCCESS;
}


    //
TInt32 CAppConnection::SendMsg(TUChar *pMsg,TUInt32 msgLen)
{
    if (m_pIfConnection)
    {
         return m_pIfConnection->SendMsg(pMsg,msgLen);
    }
    return NOT_INITIALIZED;
}

    //��ȡ������Ϣ
CConPair *CAppConnection::GetConnectionInfo()
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->GetConnectionInfo();
    }
    return NULL;
}
    //�����Ƿ���ҪNegla�㷨
TInt32 CAppConnection::NeedNoDelay(const char chOpt)
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->NeedNoDelay(chOpt);
    }
    return NOT_INITIALIZED;
}
    //��ȡ����״̬.
EnConnectionState CAppConnection::GetConnctionState()
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->GetConnctionState();
    }
    return connection_is_not_in_use;
}
    //���Ի�ȡ��δ���͵����ݵĳ���
TInt32 CAppConnection::GetPendingDataLen()
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->GetPendingDataLen();
    }
    return NOT_INITIALIZED;
}

    //��������󣬾Ϳ��Խ�IfConnectionCallBack�ͷ�.Net��������ص��ýӿ�.
TInt32 CAppConnection::Disconnect()
{
    if (m_pIfConnection)
    {
        TInt32 ret = m_pIfConnection->Disconnect();
        m_pIfConnection = NULL;
        return ret;
    }
    return SUCCESS;
}




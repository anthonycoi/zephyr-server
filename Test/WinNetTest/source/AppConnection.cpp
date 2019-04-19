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
    //呃，这儿又是一个bug，因为转的很快，time(NULL)都是相同值，所以下面的rand()都是相同值
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
    //网络层会自动从factory生成parser和crypter,请应用层对这连个东西进行设置
    //应用层应该明确知道IfParser 和 IfCryptor的实现类，并在OnConnected的时候对其进行设置.
    //如果返回值en_if_connection_callback_on_connected_release_parser 设置了，那么parser会被释放
    //如果返回值en_if_connection_callback_on_connected_release_cryptor 设置了，那么pCryptor会被释放
    //IfConnection *pIfConnection在连接实际建立的时候再传给应用层。
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

    //任何socket异常都会自动关闭网络连接
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

    //获取连接信息
CConPair *CAppConnection::GetConnectionInfo()
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->GetConnectionInfo();
    }
    return NULL;
}
    //设置是否需要Negla算法
TInt32 CAppConnection::NeedNoDelay(const char chOpt)
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->NeedNoDelay(chOpt);
    }
    return NOT_INITIALIZED;
}
    //获取连接状态.
EnConnectionState CAppConnection::GetConnctionState()
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->GetConnctionState();
    }
    return connection_is_not_in_use;
}
    //用以获取还未发送的数据的长度
TInt32 CAppConnection::GetPendingDataLen()
{
    if (m_pIfConnection)
    {
        return m_pIfConnection->GetPendingDataLen();
    }
    return NOT_INITIALIZED;
}

    //调用这个后，就可以将IfConnectionCallBack释放.Net不会继续回调该接口.
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




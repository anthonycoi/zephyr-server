#include "ConnectionMgr.h"

#include "time.h"


namespace Zephyr
{


CConnectionMgr::CConnectionMgr()
{

    m_pCryptorFactory = NULL;
    m_pListeners      = NULL;
    //m_lastRunTo = 0;
    
}

CConnectionMgr::~CConnectionMgr()
{
    //Final();
}

TInt32 CConnectionMgr::Init(TUInt32 maxConnectionNum,IfTaskMgr *pTaskMgr,IfParserFactory* pParserFactory,IfCryptorFactory *pIfCryptorfactory,TUInt32 nSendBuffSize,TUInt32 nRecvBuffSize)
{
    int result = m_conncectionPool.Init(maxConnectionNum);
    if (SUCCESS > result)
    {
        return result;
    }
    
    for (TUInt32 i = 0;i<maxConnectionNum;++i)
    {
        m_conncectionPool.GetConectionByIdx(i)->SetEventQueue(&m_netEventQueues);
        m_conncectionPool.GetConectionByIdx(i)->SetTimer(&m_nTimeNow);
        int ret = m_conncectionPool.GetConectionByIdx(i)->OnCreate(i,nSendBuffSize,nRecvBuffSize);
        if (ret < SUCCESS)
        {
            return ret;
        }
    }
    
    
   
    if (nSendBuffSize > nRecvBuffSize)
    {
        m_buffSize = nSendBuffSize;
    }
    else
    {
        m_buffSize = nRecvBuffSize;
    }
    NEW(m_pBuff,TUChar,m_buffSize);
    if (!m_pBuff)
    {
        return OUT_OF_MEM;
    }
    m_pListeners = NULL;
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2,2), &wsaData);	
    if (result != SUCCESS)
    {
        int errCode = GetLastError();
        return -errCode;
    }

    SOCKET s;	

    //
    // First open a temporary socket that we will use to create the
    // completion port.  In NT 3.51 it will not be necessary to specify
    // the FileHandle parameter of CreateIoCompletionPort()--it will
    // be legal to specify FileHandle as NULL.  However, for NT 3.5
    // we need an overlapped file handle.
    //

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if ( s == INVALID_SOCKET ) 
    {	
        return OUT_OF_MEM;
    }


    // Create the completion port that will be used by all the workers
    // threads.
    m_hCompletionPort = CreateIoCompletionPort( (HANDLE)s, NULL, 0, 0 );
    if ( m_hCompletionPort == NULL ) 
    {
        closesocket( s );
        return FAIL;
    }

    closesocket(s);


    for (TUInt32 i=0;i<NR_OF_NET_WORKER;i++)
    {
        m_netWorker.Init(m_hCompletionPort,&m_conncectionPool,&m_netEventQueues,m_buffSize);
        result = pTaskMgr->AddTask(&m_netWorker,normal_task);
        if (result < SUCCESS)
        {
            return result;
        }
    }
    
    result = m_connector.Init(128,m_hCompletionPort,&m_conncectionPool,pParserFactory,pIfCryptorfactory);
    if (result < SUCCESS)
    {
        return result;
    }
    m_pParserFactory = pParserFactory;
    m_pCryptorFactory = pIfCryptorfactory;
    
    result = m_netEventQueues.Init(maxConnectionNum);
    return result;
}

void  CConnectionMgr::Final()
{
    m_netWorker.Final();
    while (m_pListeners)
    {
        CListener *pNext = m_pListeners->GetNext();
        m_pListeners->Final();
        delete m_pListeners;
        m_pListeners = pNext;
    }
    
    
    delete m_pParserFactory;
    m_pParserFactory  = NULL;
    delete m_pCryptorFactory;
    m_pCryptorFactory = NULL;
}

TInt32 CConnectionMgr::Connect(const TChar *pRemoteIp,const TChar *pMyIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack)
{
    if (!pAppCallBack)
    {
        return NULL_POINTER;
    }
    if (m_connector.IsListFull())
    {
        return IF_NET_ERROR_CODE_TOO_MANY_PENDING_CONNECTIONGS;
    }
    TUInt32 remoteIp = inet_addr(pRemoteIp);
    TUInt32 myIp = inet_addr(pMyIp);
    CConPair pair;
    pair.Init(remoteIp,myIp,remotePort,myPort);
    return m_connector.Connect(&pair,(IfConnectionCallBack*)pAppCallBack);
}

TInt32 CConnectionMgr::Connect(const TUInt32 remoteIp,const TUInt32 myIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack)
{
    if (!pAppCallBack)
    {
        return NULL_POINTER;
    }
    CConPair pair;
    pair.Init(remoteIp,myIp,remotePort,myPort);
    return m_connector.Connect(&pair,(IfConnectionCallBack*)pAppCallBack);
}

TInt32 CConnectionMgr::Run(TUInt32 runCnt)
{
    TInt32 usedCnt = 0;
    CListener *pList = m_pListeners;
    m_nTimeNow = time(NULL);
    while (pList)
    {
        usedCnt += pList->Run(runCnt-usedCnt);
        if (usedCnt >= runCnt)
        {
            return usedCnt;
        }
        pList = pList->GetNext();
    }
    usedCnt += m_connector.Run((runCnt-usedCnt));
    TIOEvent *pEvent = m_netEventQueues.GetNetEvent();
    
    while(pEvent)
    {
        TIOEvent event = *pEvent;
        m_netEventQueues.ConfirmHandleNetEvent(pEvent);
        CConnection *pConnection = m_conncectionPool.GetConectionByIdx(pEvent->m_connectionIdx);
        if (pConnection)
        {
            //先确认处理了消息，这样网络层如果有新事件，也会发事件通知.
            pConnection->OnAppRecved();
            //由应用层调用，如果返回-1，则表示需要释放连接,把链接close ,并且放回connectionPool
            TInt32 ret = pConnection->AppRoutine(m_pBuff,m_buffSize);
            //不能先处理，再确认，因为，可能在处理完了，即上句执行完了，网络层又有了新事件，就会丢失.
            //pConnection->OnAppRecved();
            pConnection->OnAppHandled();

            if (ret < SUCCESS)
            {
                //pConnection->CloseConnection();
                m_conncectionPool.ReleaseItem(pConnection);
            }
        }
        ++ usedCnt;
        if(usedCnt > runCnt)
        {
            break;
        } 
        pEvent = m_netEventQueues.GetNetEvent();
    }
    return usedCnt;
}
//
//TInt32 CConnectionMgr::Disconnect(TUInt32 connectionIdx)
//{
//    if (connectionIdx > m_maxConnectionNum)
//    {
//        return OUT_OF_RANGE;
//    }
//    switch (m_pConnectionPool[connectionIdx].GetConnectionState())
//    {
//        
//        case connection_is_not_in_use:
//        {
//            return CONNECTION_NOT_ESTABLISHED;
//        }
//        break;
//        case connection_is_trying:
//        {
//            m_pConnectionPool[connectionIdx].Disconnect();
//            return SUCCESS;
//        }
//        break;
//        case connection_is_established:
//        {
//            m_pConnectionPool[connectionIdx].Disconnect();
//            return SUCCESS;
//        }
//        break;
//        case connection_is_broken:
//        {
//            return SUCCESS;
//        }
//        break;
//        default:
//        {
//            return CONNECTION_IN_WRONG_STATE;
//        }
//    }
//}

void *CConnectionMgr::Listen(TUInt32 nIp,TUInt16 nPort,TUInt16 nMaxConnection,void *pIfCallBack)
{
    CListener *p = new CListener();
    if (NULL == p)
    {
        return 0;
    }
    TInt32 ret = p->Init(m_hCompletionPort,nIp, nPort, nMaxConnection,(IfListenerCallBack*)pIfCallBack,&m_conncectionPool,m_pParserFactory,m_pCryptorFactory);
    if (SUCCESS > ret)
    {
        p->Final();
        delete p;
        return 0;
    }
    p->AttachToList(m_pListeners);
    m_pListeners = p;
    return p;
}


void *CConnectionMgr::Listen(const TChar *pIp,TUInt16 port,TUInt16 maxConnection,void *pIfCallBack)
{
    CListener *p = new CListener();
    if (NULL == p)
    {
        return 0;
    }
    TUInt32 myIp = 0;
    if (pIp != NULL)
    {
        myIp = inet_addr(pIp);
    }
    TInt32 ret = p->Init(m_hCompletionPort,myIp, port, maxConnection,(IfListenerCallBack*)pIfCallBack,&m_conncectionPool,m_pParserFactory,m_pCryptorFactory);
    if (SUCCESS > ret)
    {
        p->Final();
        delete p;
        return 0;
    }
    p->AttachToList(m_pListeners);
    m_pListeners = p;
    return p;
}

TInt32 CConnectionMgr::StopListening(void *pListeningPointer)
{
    CListener *pListener = m_pListeners;
    CListener *pLast=NULL;
    while(pListener)
    {
        if (pListener == (pListeningPointer))
        {
            if(pLast)
            {
                pLast->AttachToList(pListener->GetNext());
                delete pListener;
                return SUCCESS;
            }
            else
            {
                m_pListeners = pListener->GetNext();
                delete pListener;
                return SUCCESS;
            }
        }
        else
        {
            pLast = pListener;
            pListener = pListener->GetNext();
        }
    }
    return OUT_OF_RANGE;
}


}

#include "Connector.h"
#include "ConnectionMgr.h"
#include "winsock2.h"
#include "SysMacros.h"
namespace Zephyr
{


CConnector::CConnector()
{
    m_maxPendingConnection      = 0;
    m_connected                 = 0;
    m_failed                    = 0;
    //IfNetApp        *m_pIfNetApp;
    m_hCompletionPort           = NULL;

    m_pConnectionPool           = NULL;;

    m_pCryptorFactory           = NULL;;

    m_pParserFactory            = NULL;
}

CConnector::~CConnector()
{

}

TInt32 CConnector::Init(TInt32 maxPendingConnections,HANDLE completionPort,ItemClassPool<CConnection> *pConnectionPool,IfParserFactory *pParserFactory,IfCryptorFactory *pCryptorFactory)
{
    m_maxPendingConnection = maxPendingConnections;
    m_connected          = 0;
    m_failed             = 0;
    if (maxPendingConnections > FD_SETSIZE*3)
    {
        return OUT_OF_RANGE;
    }
    TInt32 nRet = m_tPool.InitPool(maxPendingConnections);
    if (nRet < SUCCESS)
    {
        return nRet;
    }
    m_pendingSocket.Init(&m_tPool);
    m_hCompletionPort = completionPort;
    m_pParserFactory  = pParserFactory;
    m_pCryptorFactory = pCryptorFactory;
    m_pConnectionPool = pConnectionPool;
    return SUCCESS;
}

TInt32 CConnector::Run(const TUInt32 runCnt)
{
    TUInt32 usedCnt = 0;
    if (!m_pendingSocket.GetActivedSize())
    {
        return SUCCESS;
    }
    fd_set wset,exceptSet;
    FD_ZERO(&wset);
    FD_ZERO(&exceptSet);
    
    TplNode<CConnectingList,SOCKET>::Iterator it(m_pendingSocket.GetRoot());
    
    while ((usedCnt < runCnt) && (!it.IsNull()))
    {
        //TplMap<CConnectingList,SOCKET>::Iterator it(m_pendingSocket.GetRoot());
        //if ()
        //FD_ZERO(&wset);
        FD_ZERO(&wset);
        while (!it.IsNull())
        {
            (it->m_tryTimes++);
//             if (it->m_tryTimes > 100)
//             {
//                 OnDisconnected(it->m_pConnection);
//                 CConnectingList *pList = it.GetItem();
//                 ++it;
//                 m_pendingSocket.ReleaseItem(pList);
//                 continue;
//             }
            wset.fd_array[wset.fd_count] = it->m_pConnection->GetSocket();
            ++wset.fd_count;
            ++it;
            if (wset.fd_count >= FD_SETSIZE)
            {
                break;
            }
        }
        if (!it.IsNull())
        {
            printf("Too much socket?!\n\t");
        }
        usedCnt += wset.fd_count;
        exceptSet = wset;
        struct timeval tm;
        tm.tv_sec=0;
        tm.tv_usec=0;
        int nfds = select(wset.fd_count,NULL,&wset,&exceptSet,&tm);
        //TInt32 usedCnt = 0;
        switch (nfds) 
        {
        case -1:
            {
                int errCode = WSAGetLastError();
                return -errCode;
            }
        case 0: // nothing happened!
            break;
        default:
            {
                for(TUInt32 i=0;i<wset.fd_count;++i)
                {
                    CConnectingList *pCItem = m_pendingSocket.GetItemByKey(wset.fd_array[i]);
                    if (pCItem)
                    {
                        OnConnectionEstablish(pCItem->m_pConnection);
                        m_pendingSocket.RemoveFromTree(pCItem);
                        m_pendingSocket.ReleaseItem(pCItem);
                    }
                    else
                    {
                        //?
                        printf("[CConnector::Run] Find unexisted socket!");
                    }
                }
                for(TUInt32 i=0;i<exceptSet.fd_count;++i)
                {
                    CConnectingList *pCItem = m_pendingSocket.GetItemByKey(exceptSet.fd_array[i]);
                    if (pCItem)
                    {
                        OnDisconnected(pCItem->m_pConnection);
                        m_pendingSocket.RemoveFromTree(pCItem);
                        int ret = m_pendingSocket.ReleaseItem(pCItem);
                        if (ret < SUCCESS)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        //?
                        printf("[CConnector::Run] Find unexisted socket!");
                    }
                }
            }
        }
    }
    
	return usedCnt;
}

TInt32 CConnector::Connect(CConPair *pPair,IfConnectionCallBack *pAppCallBack)
{
    //不查了，下面的GetItem会保证
//     if (m_pendingSocket.IsFull())
//     {
//         return OUT_OF_MEM;
//     }
    CConnection *pNew = m_pConnectionPool->GetItem();
    if(!pNew)
    {
        return OUT_OF_MEM;
    }
    
    SOCKET socket = SOCKET_ERROR;
    
   if((socket=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED))==-1)
   {
        //m_socket = NULL;
        m_pConnectionPool->ReleaseItem(pNew);
        return OUT_OF_MEM;
    }
    
//     if (SetSocketOptions() != SUCCESS)
//     {
//         closesocket(socket);
//         return FAIL;
//     }
    SOCKADDR_IN remoteAdd;
    SecureZeroMemory(&remoteAdd,sizeof(sockaddr));

    SOCKADDR_IN myAdd;
    SecureZeroMemory(&myAdd,sizeof(SOCKADDR_IN));
    myAdd.sin_port = htons(pPair->GetMyPort());

    // Fill in the rest of the address structure
    myAdd.sin_family = AF_INET;
    myAdd.sin_addr.s_addr = pPair->GetMyIp();

    // bind our name to the socket
    TInt32 ret = bind(socket, 
        (LPSOCKADDR)&myAdd, 
        sizeof(struct sockaddr));
    if(ret   ==   SOCKET_ERROR)   
    {   
        closesocket(socket);
        m_pConnectionPool->ReleaseItem(pNew);
        return FAIL;
    }

    remoteAdd.sin_family=AF_INET;
    remoteAdd.sin_addr.s_addr = pPair->GetRemoteIp();
    remoteAdd.sin_port = htons(pPair->GetRemotePort());
    
    //没有指定本地IP和本地port,给补上
    if ((0 == pPair->GetMyPort()) || (INADDR_ANY == pPair->GetMyIp()))
    {
        sockaddr_in myAdd;
        int     myNameLen = sizeof(sockaddr_in);
        ret = getsockname(socket,(sockaddr *)&myAdd,&myNameLen);;
        if (ret == SOCKET_ERROR)
        {
            m_pConnectionPool->ReleaseItem(pNew);
            return (-((TInt32)WSAGetLastError()));
        }
        pPair->Init(pPair->GetRemoteIp(),myAdd.sin_addr.s_addr,pPair->GetRemotePort(),ntohs(myAdd.sin_port));
    }
    
    IfParser *pParser;
    TUInt32 connectionIdx = pNew->GetConnectionIdx();
    if (m_pParserFactory)
    {
        pParser = m_pParserFactory->GetParser(pPair,connectionIdx);
    }
    else
    {
        pParser = NULL;
    }
    IfCryptor* pCryptor;
    if (m_pCryptorFactory)
    {
        pCryptor = m_pCryptorFactory->GetCryptor(pPair,connectionIdx);
    }
    else
    {
        pCryptor = NULL;
    }
    
    ret = pNew->Init(socket,pPair,pAppCallBack,pParser,pCryptor);
    pNew->SetConnectionType(connection_is_passive);
    if(ret   ==   SOCKET_ERROR)   
    {   
        closesocket(socket);
        m_pConnectionPool->ReleaseItem(pNew);
        return FAIL;
    }
    
    ret = connect(socket,(struct sockaddr *)&remoteAdd,sizeof(sockaddr));
    
    if(ret == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        if (WSAEWOULDBLOCK == error)
        {
            TInt32 ret = AddToPendingList(pNew);
            if (SUCCESS > ret)
            {
                m_pConnectionPool->ReleaseItem(pNew);
                return ret;
            }
            return WSAEWOULDBLOCK;
        }
        else
        {
            m_pConnectionPool->ReleaseItem(pNew);
            return (-ret);
        }
    }
    else
    {
        //直接成功了？
        OnConnectionEstablish(pNew);
        return ret;
    }
}


TInt32 CConnector::AddToPendingList(CConnection *pConnection)
{
    TInt32 result = SUCCESS;
    SOCKET socket = pConnection->GetSocket();
    if (m_pendingSocket.GetItemByKey(socket)) //已经加过了？！
    {
        return FAIL;
    }
    CConnectingList *pList = m_pendingSocket.PrepareItem();
    if (pList && (result == SUCCESS))
    {
        pList->m_key = pConnection->GetSocket();
        pList->m_pConnection = pConnection;
        pList->m_tryTimes = 0;
        m_pendingSocket.AddInTree(pList);
    }
    else
    {
        return result;
    }
    pConnection->OnTryConnecting();
    return SUCCESS;
}

void CConnector::OnConnectionEstablish(CConnection *pConnection)
{
    HANDLE h = CreateIoCompletionPort((HANDLE) pConnection->GetSocket(), m_hCompletionPort, (ULONG_PTR)(pConnection), 0);
    TInt32 result;
    if (h != m_hCompletionPort)
    {
        result = FAIL;
    }
    else
    {
        result = pConnection->OnConnected();
    }
    if (SUCCESS > result)
    {
        OnDisconnected(pConnection);
//         pConnection->CloseConnection();
//         pConnection->OnDisconnected();
//         m_pConnectionPool->ReleaseConnection(pConnection);
        return ;
    }
    //pConnection->OnConnected();
}

void CConnector::OnDisconnected(CConnection *pConnection)
{
    pConnection->CloseConnection();
    pConnection->OnDisconnected();
    m_pConnectionPool->ReleaseItem(pConnection);
}

}


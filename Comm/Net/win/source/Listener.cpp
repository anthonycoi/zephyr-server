#include "Listener.h"
#include "NetApi.h"

namespace Zephyr
{

CListener::CListener()
{
    m_myIp = 0;
    m_listeningPort = 0;
    m_maxAcceptNr = 0;
    m_listeningSocket = SOCKET_ERROR;
    m_pListenerCallBack = NULL;
    m_pConnectionPool   = NULL;
}
CListener::~CListener()
{
    if (SOCKET_ERROR != m_listeningSocket)
    {
        closesocket(m_listeningSocket);
    }
    
}
TInt32 CListener::Init(HANDLE completionPort,TUInt32 myIp,TUInt16 listeningPort,TUInt16 maxAcceptNr,IfListenerCallBack *pListenerCallBack,ItemClassPool<CConnection> *pConnectionPool,IfParserFactory *pParserFactory,IfCryptorFactory *pCryptorFactory)
{
    m_compeltionPort = completionPort;
    m_myIp = myIp;
    m_listeningPort = listeningPort;

    m_pNext         = NULL;
    
    m_listeningSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (SOCKET_ERROR == m_listeningSocket)
    {
        int errCode = GetLastError();

        return -errCode;
    }

    TInt32 ret = SetSocketOptions(m_listeningSocket);
    if (SUCCESS > ret)
    {
        closesocket(m_listeningSocket);
        return ret;
    }
    
    SOCKADDR_IN myAdd;
    SecureZeroMemory(&myAdd,sizeof(SOCKADDR_IN));
    myAdd.sin_port = htons(listeningPort);

	// Fill in the rest of the address structure
	myAdd.sin_family = AF_INET;
	myAdd.sin_addr.s_addr = myIp;

	// bind our name to the socket
	ret = bind(m_listeningSocket, 
		(LPSOCKADDR)&myAdd, 
		sizeof(struct sockaddr));
    if(ret   ==   SOCKET_ERROR)   
    {   
        closesocket(m_listeningSocket);
        m_listeningSocket = 0;
        return FAIL;
    }
    ret = listen(m_listeningSocket, maxAcceptNr);
    
    if ( ret == SOCKET_ERROR )
	{
		closesocket(m_listeningSocket);
		return FALSE;
	}
    m_maxAcceptNr            = maxAcceptNr;
    m_pListenerCallBack      = pListenerCallBack;
    m_pConnectionPool        = pConnectionPool;
    m_pParserFactory         = pParserFactory;
    m_pCryptorFactory        = pCryptorFactory;
	return SUCCESS;
    
}

TInt32 CListener::Run(TInt32 cnt)
{
    TInt32 usedCnt = HasNewConnection();
    SOCKET acceptedSocket = SOCKET_ERROR;
    int nLen = sizeof(SOCKADDR_IN);
    for (int i =0;i<usedCnt;++i)
    {
        acceptedSocket = WSAAccept(m_listeningSocket,
            NULL,
            &nLen,0,0);
        if (SOCKET_ERROR != acceptedSocket)
        {
//             TInt32 ret = SetSocketOptions(acceptedSocket);
//             if (SUCCESS > ret)
//             {
//                 closesocket(ret);
//                 continue;
//             }
            TInt32 result = SetSocketOptions(acceptedSocket);
            if (SUCCESS > result)
            {
                closesocket(acceptedSocket);
                continue;
            }
            
            
            CConnection *pNew = m_pConnectionPool->GetItem();
            if (!pNew)
            {
                closesocket(acceptedSocket);
                return usedCnt;
            }
            CConPair pair;
            GetConnPair(acceptedSocket,pair);
            IfConnectionCallBack *pAppCallBack = m_pListenerCallBack->OnNewConnection(&pair);
            if (!pAppCallBack)
            {
                closesocket(acceptedSocket);
                continue;
            }
            IfParser  *pParser;
            if (m_pParserFactory)
            {
                pParser = m_pParserFactory->GetParser(&pair,pNew->GetConnectionIdx());
            }
            else
            {
                pParser = NULL;
            }
            IfCryptor *pCryptor;
            if (m_pCryptorFactory)
            {
                 pCryptor = m_pCryptorFactory->GetCryptor(&pair,pNew->GetConnectionIdx());
            }
            else
            {
                pCryptor = NULL;
            }
            TInt32 ret = pNew->Init(acceptedSocket,&pair,pAppCallBack,pParser,pCryptor);
            pNew->SetConnectionType(connection_is_postive);
            if (SUCCESS > ret)
            {
                pNew->CloseConnection();
                m_pConnectionPool->ReleaseItem(pNew);
                continue;
            }
            HANDLE h = CreateIoCompletionPort((HANDLE) acceptedSocket, m_compeltionPort, (ULONG_PTR)(pNew), 0);

            if (h != m_compeltionPort)
            {
                closesocket(acceptedSocket);
                pNew->OnDisconnected();
                m_pConnectionPool->ReleaseItem(pNew);
                continue;
            }
            
            ret = pNew->OnConnected();
            if (SUCCESS > ret)
            {
                pNew->CloseConnection();
                m_pConnectionPool->ReleaseItem(pNew);
                continue;
            }
        }
    }
    return usedCnt;
}
    
SOCKET CListener::Accept()
{
    SOCKET acceptedSocket = SOCKET_ERROR;
    int nLen = sizeof(SOCKADDR_IN);
    acceptedSocket = WSAAccept(m_listeningSocket,
                               NULL,
                               &nLen,0,0); 
    return acceptedSocket;
}

TInt32 CListener::HasNewConnection()
{
    int nfds = 1;
    fd_set rset;
    FD_ZERO(&rset);

    struct timeval tm;
    tm.tv_sec=0;
    tm.tv_usec=0;

    FD_SET(m_listeningSocket,&rset);

    nfds = select(nfds, &rset,NULL,NULL,&tm);
    switch (nfds)
    {
        case 0:
        {
            return 0;
        }
        case -1:
        {
            return (-((TInt32)GetLastError()));
        }
        default:
        {
            return nfds;
        }
    }
}

}

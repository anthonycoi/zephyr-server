/*-------------------------------------------------------------
 Copyright (C)| 
 File: Listener.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  ¼àÌýÆ÷
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_LISTENER_H__
#define __ZEPHYR_LISTENER_H__

#include "TypeDef.h"
#include "IfNetData.h"
#include "SysMacros.h"
#include "ConnectionPool.h"
#include "IfNetApp.h"
#include "IfCryptorFactory.h"
#include "IfParserFactory.h"
//#include "winsock2.h"

namespace Zephyr
{
    
class CListener
{
private:
    TUInt32     m_myIp;
    TUInt16     m_listeningPort;
    TUInt16     m_maxAcceptNr;
    SOCKET      m_listeningSocket;
    CListener   *m_pNext;
    IfListenerCallBack  *m_pListenerCallBack;
    
    ItemClassPool<CConnection> *m_pConnectionPool;
    IfCryptorFactory *m_pCryptorFactory;
    IfParserFactory  *m_pParserFactory;
    HANDLE          m_compeltionPort;
public:
    CListener();
    ~CListener();
    TInt32 Init(HANDLE completionPort,TUInt32 myIp,TUInt16 listeningPort,TUInt16 maxAcceptNr,IfListenerCallBack *pListenerCallBack,ItemClassPool<CConnection> *pConnectionPool,IfParserFactory *pParserFactory,IfCryptorFactory *pCryptorFactory);
    void   Final()
    {
        return;
    }
    TInt32 Run(TInt32 cnt);
    SOCKET Accept();
    TInt32 HasNewConnection();
    void AttachToList(CListener *pLast)
    {
        m_pNext =  pLast;
    }
    CListener *GetNext()
    {
        return m_pNext;
    }
};

}
#endif
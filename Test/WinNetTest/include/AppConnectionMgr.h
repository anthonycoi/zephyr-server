#ifndef __ZEPHYR_COMM_WIN_NET_TEST_APP_CONNECTION_MGR_H__
#define __ZEPHYR_COMM_WIN_NET_TEST_APP_CONNECTION_MGR_H__

#include "IfNetApp.h"
#include "AppConnection.h"
#include "TypeDef.h"

using namespace Zephyr;

class CAppConnectionMgr : public IfListenerCallBack
{
private:
    CAppConnection  *m_pConnectionPool;
    CAppConnection  *m_pFree;
    CAppConnection  *m_pUsed;
    TInt32          m_connectionNr;
public:

    virtual IfConnectionCallBack *OnNewConnection(CConPair *pPair);
    TInt32  Init(TInt32 maxConnectionNr);
    CAppConnection *GetConnection();
    int GetConnectionNr()
    {
        return m_connectionNr;
    }
    CAppConnection *GetConnectionByIdx(int idx)
    {
        return m_pConnectionPool + idx;
    }
    void ReleaseConnection(CAppConnection *pConnection);
    void AddToUsedList(CAppConnection *pItem);
};

#endif

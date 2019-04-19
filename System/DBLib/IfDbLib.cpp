#include "IfDbLib.h"
#include "DBLib.h"
#include "MySQLDatabase.h"
namespace DBLib
{

IfTrascationWorkThreadMgr *__stdcall GetMysqlWorkThreadMgr(const char* pszConnectStr,int ThreadCount,int QueueSize,unsigned int Flag)
{
    CDBTransationManager *pMgr(0);
    CMySQLDatabase *pMysqlDb(0);
    try
    {
        pMgr = new CDBTransationManager();

        pMysqlDb = new CMySQLDatabase();
    }
    catch (...)
    {
    }
    if ((!pMgr) || (!pMysqlDb))
    {
        if (pMgr)
        {
            delete pMgr;
            pMysqlDb = 0;
        }
        if (pMysqlDb)
        {
            delete pMysqlDb;
            pMysqlDb = 0;
        }
        return 0;
    }
    bool bSuccess = pMgr->Init(pMysqlDb,pszConnectStr,ThreadCount,QueueSize,Flag);
    if (bSuccess)
    {
        return pMgr;
    }
    return 0;
}

void __stdcall ReleaseMgr(IfTrascationWorkThreadMgr *pIfMgr)
{
    CDBTransationManager *pMgr = dynamic_cast<CDBTransationManager *>(pIfMgr);
    if (pMgr)
    {
        IDatabase *pIDB = pMgr->GetDB();
        if (pIDB)
        {
            CMySQLDatabase *pMysqlDB = dynamic_cast<CMySQLDatabase *>(pIDB);
            if (pIDB)
            {
                delete pIDB;
            }
        }
        delete pMgr;
    }
    else
    {
        printf("pIfMgr is incorrect, Wrong type!");
    }
    return;
}

}

#ifndef __ZEPHYR_PUBLIC_IF_DB_LIB_H__
#define __ZEPHYR_PUBLIC_IF_DB_LIB_H__
#include "DBTransaction.h"

#define DEFAULT_TRANS_QUEUE		128
#define CONNECTION_TEST_TIME	60000
#define MAX_CONNECT_STRING		256

#define DEFAULT_TRANS_THREAD	4
#define DEFAULT_PROCESS_LIMIT	32
#define PERFORMANCE_COUNT_TIME	(5*60*1000)

namespace DBLib
{

class IfTrasactionWorkThread
{
public:
    virtual bool AddTransaction(CDBTransaction * pDBTansaction) = 0;
    
    virtual unsigned int GetQueueLen() = 0;
    virtual unsigned int GetFinishQueueLen() = 0; 
};

class IfTrascationWorkThreadMgr
{
public:
    virtual  IfTrasactionWorkThread *GetThread() = 0;
    virtual  IfTrasactionWorkThread *GetThread(int nIdx) = 0;
    //不用ReleaseThread,原来就是混用的，不用释放
    //virtual  void ReleaseThread() = 0;
    virtual int Update(int ProcessLimit) = 0;
};

IfTrascationWorkThreadMgr  * __stdcall GetMysqlWorkThreadMgr(const char* pszConnectStr,int ThreadCount=DEFAULT_TRANS_THREAD,int QueueSize=DEFAULT_TRANS_QUEUE,unsigned int Flag=0);

void __stdcall ReleaseMgr(IfTrascationWorkThreadMgr *IfMgr);
}
#endif

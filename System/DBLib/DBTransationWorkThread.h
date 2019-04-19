#ifndef __DB_LIB_DB_TRANSATION_WORK_THREAD_H__
#define __DB_LIB_DB_TRANSATION_WORK_THREAD_H__

#include "DBLib.h"


namespace DBLib
{



class CDBTransationManager;

class CDBTransationWorkThread : public IfTrasactionWorkThread
{
protected:
	CDBTransationManager *				m_pManager;
	IDBConnection *						m_pConnection;
	char								m_ConnectString[MAX_CONNECT_STRING];
	CThreadSafeList<CDBTransaction *>	m_TransQueue;
	CThreadSafeList<CDBTransaction *>	m_FinishTransQueue;
	CEasyTimer							m_ConnectionTestTimer;
    
    volatile        int                 m_nRun;
	
public:
	CDBTransationWorkThread(CDBTransationManager * pManager);
	~CDBTransationWorkThread(void);

	void Destory();

	bool Init(IDBConnection * pConnection,LPCTSTR ConnectStr,int QueueSize=DEFAULT_TRANS_QUEUE);
	virtual bool AddTransaction(CDBTransaction * pDBTansaction);
	CDBTransaction *PopFinishTransaction();

	virtual unsigned int GetQueueLen()
	{
		return m_TransQueue.GetObjectCount();
	}
	virtual unsigned int GetFinishQueueLen()
	{
		return m_FinishTransQueue.GetObjectCount();
	}
public:
    void SafeTerminate(int timeout);
    int Start();

protected:
    static void Run(void *pArg);
	virtual BOOL OnStart();	
	virtual BOOL OnRun();
	virtual void OnBeginTerminate();
};

}

#endif

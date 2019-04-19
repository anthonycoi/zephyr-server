#ifndef __DB_LIB_DB_TRASATION_MANAGER_H__
#define __DB_LIB_DB_TRASATION_MANAGER_H__


#include "DBLib.h"


namespace DBLib
{





class CDBTransationManager : public IfTrascationWorkThreadMgr
{
public:
	enum FLAG
	{
		DBTM_FLAG_LOG_PERFORMANCE=1,
	};
protected:
	std::vector<CDBTransationWorkThread *>	m_WorkThreads;
	IDatabase *								m_pDatabase;
	UINT									m_Flag;

	CEasyTimer								m_PerformanceCountTimer;
	volatile UINT							m_ExecTimes;
	volatile UINT							m_RealExecTime;

	float									m_AvgExecTime;
	float									m_ExecTimesPerSec;

	
	
	
public:
	CDBTransationManager(void);
	~CDBTransationManager(void);

    IDatabase *GetDB()
    {
        return m_pDatabase;
    }
	void Destory();

	bool Init(IDatabase * pDatabase,LPCTSTR szConnectStr,int ThreadCount=DEFAULT_TRANS_THREAD,int QueueSize=DEFAULT_TRANS_QUEUE,UINT Flag=0);

	void ModifyFlag(UINT Add,UINT Remove)
	{
		m_Flag|=Add;
		m_Flag&=~Remove;
	}

	CDBTransationWorkThread * AllocWorkThread();
	
	bool AddTransaction(CDBTransaction * pDBTansaction);

    virtual  IfTrasactionWorkThread *GetThread();
    virtual  IfTrasactionWorkThread *GetThread(int nIdx);
    
	virtual int Update(int ProcessLimit=DEFAULT_PROCESS_LIMIT);

	bool IsIdle();

	void AddExecTime(UINT ExecTime)
	{
		m_ExecTimes++;
		m_RealExecTime+=ExecTime;
	}

	float GetAvgExecTime()
	{
		return m_AvgExecTime;
	}
	float GetExecTimesPerSec()
	{
		return m_ExecTimesPerSec;
	}

};

}

#endif

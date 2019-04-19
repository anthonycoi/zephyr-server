#include "DBLib.h"


namespace DBLib
{


CDBTransationManager::CDBTransationManager(void)
{
	m_pDatabase=NULL;
	m_ExecTimes=0;
	m_RealExecTime=0;
	m_AvgExecTime=0;
	m_ExecTimesPerSec=0;
}

CDBTransationManager::~CDBTransationManager(void)
{
	Destory();
}

void CDBTransationManager::Destory()
{
	for(int i=0;i<(int)m_WorkThreads.size();i++)
	{
		if(m_WorkThreads[i])
			m_WorkThreads[i]->SafeTerminate(DEFAULT_THREAD_TERMINATE_TIME);
		SAFE_DELETE(m_WorkThreads[i]);
	}
	m_WorkThreads.clear();
}

bool CDBTransationManager::Init(IDatabase * pDatabase,LPCTSTR szConnectStr,int ThreadCount,int QueueSize,UINT Flag)
{
	if(pDatabase==NULL)
		return false;
	Destory();
	m_pDatabase=pDatabase;
	m_Flag=Flag;

	for(int i=0;i<ThreadCount;i++)
	{
		IDBConnection * pConnection=m_pDatabase->CreateConnection();
		if(pConnection->Connect(szConnectStr)!=DBERR_SUCCEED)
		{
			PrintDBLog(0xff,"数据库无法连接，但初始化继续");
		}
		CDBTransationWorkThread * pThread=new CDBTransationWorkThread(this);
		if(!pThread->Init(pConnection,szConnectStr,QueueSize))
		{
			SAFE_DELETE(pThread);
			return false;
		}
		m_WorkThreads.push_back(pThread);

	}


	m_PerformanceCountTimer.SaveTime();

	PrintDBLog(0xff,"一共建立了%d个工作线程",ThreadCount);

	return true;
}

CDBTransationWorkThread * CDBTransationManager::AllocWorkThread()
{
	CDBTransationWorkThread * pThread=NULL;
	UINT Len=0xffffffff;
	for(int i=0;i<(int)m_WorkThreads.size();i++)
	{
		if(m_WorkThreads[i]->GetQueueLen()<Len)
		{
			pThread=m_WorkThreads[i];
			Len=m_WorkThreads[i]->GetQueueLen();
		}
	}
	return pThread;
}

bool CDBTransationManager::AddTransaction(CDBTransaction * pDBTansaction)
{
	CDBTransationWorkThread * pThread=AllocWorkThread();
	if(pThread)
	{
		return pThread->AddTransaction(pDBTansaction);
	}
	else
		return false;
}



int CDBTransationManager::Update(int ProcessLimit)
{
	int ProcessCount=0;
	while(ProcessCount<ProcessLimit)
	{
		int Count=0;
		for(int i=0;i<(int)m_WorkThreads.size();i++)
		{
			CDBTransaction * pDBTansaction=m_WorkThreads[i]->PopFinishTransaction();
			if(pDBTansaction)
			{
				pDBTansaction->OnFinish();
                //不再自动删除，由使用者保证一定要管理内存，因为这个Transcation是由使用者给进来的.
				//SAFE_DELETE(pDBTansaction);
				ProcessCount++;
				Count++;
			}
		}
		if(Count==0)
			break;
	}

	if(m_PerformanceCountTimer.IsTimeOut(PERFORMANCE_COUNT_TIME))
	{
		m_PerformanceCountTimer.SaveTime();
		m_ExecTimesPerSec=(float)m_ExecTimes*1000/PERFORMANCE_COUNT_TIME;
		if(m_ExecTimes)
			m_AvgExecTime=(float)m_RealExecTime/m_ExecTimes;
		else
			m_AvgExecTime=0;
		m_RealExecTime=0;
		m_ExecTimes=0;
		if(m_Flag&DBTM_FLAG_LOG_PERFORMANCE)
		{
			PrintDBLog(0xff,"平均执行时间=%g毫秒,每秒执行次数%g",m_AvgExecTime,m_ExecTimesPerSec);
		}
	}

	return ProcessCount;
}

bool CDBTransationManager::IsIdle()
{
	int Len=0;
	for(int i=0;i<(int)m_WorkThreads.size();i++)
	{
		Len+=m_WorkThreads[i]->GetQueueLen()+m_WorkThreads[i]->GetFinishQueueLen();
	}
	return Len==0;
}

IfTrasactionWorkThread *CDBTransationManager::GetThread()
{
    return AllocWorkThread();
}

IfTrasactionWorkThread *CDBTransationManager::GetThread(int nIdx)
{
    if (nIdx < m_WorkThreads.size())
    {
        return m_WorkThreads[nIdx];
    }
    return NULL;
}

}

#include "TypeDef.h"
#include "SysMacros.h"
#include "TaskMgr.h"


using namespace Zephyr;
TInt32 CTaskInfo::ApplyTask(TInt32 threadId)
{
    if (m_operatingThread != 0)
    {
        return TASK_IS_RUNING;
    }
    if(m_lock.TryLock())
    {
        if (m_operatingThread != 0)
        {
            //inpossible!
            return TASK_IS_RUNING;
        }
        m_operatingThread = threadId;
        return SUCCESS;
    }
    return TASK_IS_RUNING;
    
}
TInt32 CTaskInfo::Run(TInt32 threadId)
{
    TInt32 runCnt = 0;
    if (m_pTask)
    {
        if (SUCCESS == m_pTask->Begin(threadId))
        {
            runCnt = m_pTask->Run(threadId, (TASK_RUN_COUNT<<m_priority));
            m_pTask->End(threadId);
        }
    }
    return runCnt;
}
    //--m_operatingThread
TInt32 CTaskInfo::QuitTask()
{
    m_operatingThread = 0;
    m_lock.Unlock();
    
	return SUCCESS;
}


void CWorkerControler::Sleep()
{
    m_tLock.Lock();
    ++m_nSleepingNr;
    m_tLock.Unlock();
    m_tPAndC.RequireFetchProduct();
    m_tLock.Lock();
    --m_nSleepingNr;
    m_tLock.Unlock();
}

void CWorkerControler::WakeUp()
{
    if (m_nSleepingNr>0)
    {

        if ((m_nCpuNr + m_nSleepingNr) > m_nWorkerNr)
        {
            m_tPAndC.OnProduced();
        }
    }
}

void  CTaskWorkers::Run(void *pArg)
{
    CTaskWorkers *pThis = (CTaskWorkers *)pArg;
    pThis->Loop();
    //return SUCCESS;
}
void CTaskWorkers::Loop()
{
    if (!m_pTaskInfo)
    {
        return;
    }
    while (m_workerThreadHandle)
    {
        bool bIsAllTaskDone = true;
        for (TInt32 i =0;i<MAX_TASK_NR_IN_MGR;++i)
        {
            int taskIdx = (i+m_threadIdx)%MAX_TASK_NR_IN_MGR;
            if (TRUE == m_pTaskInfo[taskIdx].IsUsed())
            {
                if (SUCCESS == m_pTaskInfo[taskIdx].ApplyTask(m_threadIdx))
                {
                    TInt32 nUsedCnt = m_pTaskInfo[taskIdx].Run(m_threadIdx);
                    m_pTaskInfo[taskIdx].QuitTask();
                    if (nUsedCnt)
                    {
                        bIsAllTaskDone = false;
                        m_pCenter->WakeUp();
                    }
                    
                }
            }
        }
        if (bIsAllTaskDone)
        {
            if(m_threadIdx)
            {
                m_pCenter->Sleep();
                //Sleep(m_threadSleepGap + MIN_SYSTEM_SLEEP_TIME);
            }
        }
    }
    m_pTaskInfo = NULL;
}

TInt32  CTaskWorkers::Start(CTaskInfo *pInfo,TInt32 threadIdx,TInt32 sleepGap,CWorkerControler *pCenter)
{
    m_pTaskInfo = pInfo;
    m_workerThreadHandle = 1;
    m_pCenter = pCenter;
    
	unsigned long p = _beginthread( CTaskWorkers::Run,(512*1024), (void*)this);  
	
    if (p)
    {
        m_workerThreadHandle = p;
        m_threadIdx = threadIdx;
        m_threadSleepGap = sleepGap;
        return SUCCESS;
    }
    m_threadIdx = 0;
    return FAIL;
}

TInt32  CTaskWorkers::Stop()
{
    m_workerThreadHandle = 0;
    while (m_pTaskInfo)
    {
        Sleep(15);
    }
    return SUCCESS;
}

CTaskWorkers::~CTaskWorkers()
{
    Stop();
}

CTaskMgr::CTaskMgr()
{
    m_pWorkers = NULL;
    m_nrOfWorker = 0;
}


TInt32 CTaskMgr::StartWorking(TInt32 nrOfWorkersNeeded,TInt32 nCpuNr)
{
    if (!m_nrOfWorker)
    {
        m_pWorkers = new CTaskWorkers[nrOfWorkersNeeded];
        TInt32 sleepGap = MIN_SYSTEM_SLEEP_TIME / nrOfWorkersNeeded;
        if (!sleepGap)
        {
            sleepGap = 1;
        }
        TInt32 result = SUCCESS;
        for (TInt32 i=0;i<nrOfWorkersNeeded;i++)
        {
            result = m_pWorkers[i].Start(m_taskList, i,  sleepGap,&m_tWorkerControler);
            if (result != SUCCESS)
            {
                for (TInt32 j=0;j<i;j++)
                {
                    m_pWorkers[j].Stop();
                }
                return OUT_OF_MEM;
            }
        }
    }
    m_nrOfWorker = nrOfWorkersNeeded;
    m_tWorkerControler.m_nWorkerNr = nrOfWorkersNeeded;
    m_tWorkerControler.m_nCpuNr = nCpuNr;
    return SUCCESS;
}
TInt32 CTaskMgr::StopWorking()
{
    if (m_pWorkers)
    {
        for (TInt32 j=0;j<m_nrOfWorker;j++)
        {
            m_pWorkers[j].TryStop();
        }
        for (TInt32 j=0;j<m_nrOfWorker;j++)
        {
            m_pWorkers[j].Stop();
        }
    }
    delete [] m_pWorkers;
    m_pWorkers = NULL;
    return SUCCESS;
}


TInt32 CTaskMgr::AddTask(IfTask *pTask,EnTaskPriority priority)
{
    m_listLock.Lock();
    for (TInt32 i=0;i<MAX_TASK_NR_IN_MGR;i++)
    {
        if (FALSE == m_taskList[i].IsUsed())
        {
            TInt32 result = m_taskList[i].LoadTask(pTask,priority);
            m_listLock.Unlock();
            return result;
        }
    }
    m_listLock.Unlock();
	return OUT_OF_MEM;
}
TInt32 CTaskMgr::ReleaseTask(IfTask *pTask)
{
    m_listLock.Lock();
    for (TInt32 i=0;i<MAX_TASK_NR_IN_MGR;i++)
    {
        if (pTask == m_taskList[i].GetTask())
        {
            m_taskList[i].UnloadTask();
            m_listLock.Unlock();
            return SUCCESS;
        }
    }
    m_listLock.Unlock();
	return TASK_NOT_FOUND;
}
CTaskMgr::~CTaskMgr()
{
    SAFE_DELETE_ARRAY(m_pWorkers);
}

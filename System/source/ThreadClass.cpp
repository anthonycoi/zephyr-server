#include "../include/ThreadClass.h"
#include "../../Public/include/SysMacros.h"
namespace Zephyr
{

TInt32 CThread::OnStart()
{
    return SUCCESS;
}

TInt32 CThread::Start()
{
#ifdef _WIN32
    unsigned long p = _beginthread( CThread::Loop,(512*1024), (void*)this);  

    if (p)
    {
        return SUCCESS;;
    }
#else

#endif
    return FAIL;
}

TInt32 CThread::OnStop()
{
    return SUCCESS;
}

TInt32 CThread::Stop(TBOOL bBlock)
{
    m_nContinue = 0;
    if (bBlock)
    {
        m_cStopped.Lock();
        m_cStopped.Unlock();
    }
    return SUCCESS;
}

void CThread::Loop(void *pFunc)
{
    CThread *pThread = (CThread *)pFunc;

    pThread->m_cStopped.Lock();
    int nRet = 0;
    while(pThread->m_nContinue)
    {
        nRet = pThread->Run(128);
        if (0 == nRet)
        {
            SleepT(10);
        }
    }
    pThread->m_cStopped.Unlock();
}

TInt32 CThread::Run(TInt32 runCun)
{
    return 0;
}

}
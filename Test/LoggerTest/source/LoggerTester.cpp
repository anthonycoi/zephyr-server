#include "LoggerTester.h"
#include "Mmsystem.h"
#pragma  comment(lib,"Winmm.lib")
using namespace Zephyr;


CloggeRTester::CloggeRTester()
{
    
    m_lastWriteTime = timeGetTime();
    m_totalWriteSize = 0;
    m_pLogger = NULL;
}

TInt32 CloggeRTester::Run(const TInt32 threadId,const TInt32 runCnt)
{
    unsigned long timeNow = timeGetTime();
    unsigned long gap = 0;
    if (timeNow > m_lastWriteTime)
    {
        gap = timeNow - m_lastWriteTime;
    }
    if (gap > 0)
    {
        gap *= 2;
        while (gap > 0)
        {
            gap --;
            m_pLogger->WriteLog(32,log_run,"Write one piece of testing log for the %d times at time %d, last timesi %d",gap,timeNow,m_lastWriteTime);
        }
    }
    m_lastWriteTime = timeNow;
    return SUCCESS;
}
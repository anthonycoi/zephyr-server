#ifndef __ZEPHYR_TEST_LOGGER_TESTERH__
#define __ZEPHYR_TEST_LOGGER_TESTERH__
#include "TypeDef.h"
#include "SysInc.h"
//Task
#include "TaskCenter.h"
#include "SysMacros.h"
#include "IfLoggerMgr.h"
#include "ExceptionParser.h"
using namespace Zephyr;

class CloggeRTester : public IfTask
{
public:
    unsigned long m_lastWriteTime;
    unsigned long m_totalWriteSize;
    IfLogger *m_pLogger;
    
public:
    void SetLogger(IfLogger *pLogger)
    {
        m_pLogger = pLogger;
    }
    CloggeRTester();
    
    virtual TInt32 Begin(TInt32 threadId)
    {
        return SUCCESS;
    }
    virtual TInt32 Run(const TInt32 threadId,const TInt32 runCnt);
    virtual TInt32 End(TInt32 threadId)
    {
        return SUCCESS;
    }
};

#endif

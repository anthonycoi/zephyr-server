#ifndef __ZEPHYR_LOGGER_LOGGER_MANAGER_H__
#define __ZEPHYR_LOGGER_LOGGER_MANAGER_H__
#include "Logger.h"
#include "../../../Public/Interface/Platform/include/IfFinalizer.h"
#include "../../../Public/Interface/Platform/include/IfTask.h"
#include "../../../Public/Interface/Platform/include/IfLoggerMgr.h"
#include "../../include/Lock.h"
namespace Zephyr
{

#define MAX_LOGGER_NUM 64


class CLoggerManager : public IfTask 
                     , public IfFinalizer
                     , public IfLoggerManager
{
protected:
    CLoggerManager();
    static CLoggerManager* sm_pInstance;
protected:
    //
    CLogger *m_pLogger[MAX_LOGGER_NUM];
    CLock    m_cLock;
public:
    
    static CLoggerManager *Instance()
    {
        return sm_pInstance;
    }
    static TInt32 InitInstance()
    {
        if (!sm_pInstance)
        {
            try
            {
                sm_pInstance = new CLoggerManager();
            }
            catch (...)
            {
                return OUT_OF_MEM;
            }
            
            if (sm_pInstance)
            {
                return sm_pInstance->Init();
            }
        }
        return SUCCESS;
    }
public:
    TInt32 Init();
    //default max cached log lenght is 1M.
    virtual IfFinalizer *GetLogFinalizer()
    {
        return this;
    }
    virtual TInt32 AddLogger(const TChar *pName,TInt32 loggerIdx,TUInt32 logLvlWriteFileMask = 0xFFFFFFFF,TUInt32 logLvlPrintScreenMask = 0,TUInt32 cacheLen = DEFAULT_CACHED_LOG_LENGTH);
    virtual void ReleaseLogger(IfLogger *pLogger);

    virtual void WriteLog(const TUInt32 loggerIdx,const TUInt32 logId,const TUInt32 logLvl,const TChar *__pFormat,...);
    virtual IfLogger *GetLogger(TInt32 idx)
    {
        if (idx < MAX_LOGGER_NUM)
        {
            return m_pLogger[idx];
        }
        return NULL;
    }
public:
    //to implement IfFinalizer interface
    virtual void OnException(FILE *pDump = NULL);
public:
    //Implement the IfTask::Run

    
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

}




#endif

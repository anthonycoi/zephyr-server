#include "LoggerManager.h"


namespace Zephyr
{

CLoggerManager* CLoggerManager::sm_pInstance = NULL;

CLoggerManager::CLoggerManager()
{
    Init();
}

TInt32 CLoggerManager::Init()
{
    for (TInt32 i=0;i<MAX_LOGGER_NUM;i++)
    {
        m_pLogger[i] = NULL;
    }
    return SUCCESS;
}

TInt32 CLoggerManager::AddLogger(const TChar *pName,TInt32 loggerIdx,TUInt32 logLvlWriteFileMask/* = 0xFFFFFFFF*/,TUInt32 logLvlPrintScreenMask /*= 0*/,TUInt32 cacheLen/* = DEFAULT_CACHED_LOG_LENGTH*/)
{
   // m_cLock.Lock();
    CAutoLockP autolock(m_cLock);
    if (-1 == loggerIdx)
    {
        for (int i = 0;i< MAX_LOGGER_NUM;++i)
        {
            if (NULL == m_pLogger[i])
            {
                loggerIdx = i;
                break;
            }
        }
        if (-1 == loggerIdx)
        {
            return OUT_OF_MEM;
        }
    }
    else
    {
        if (loggerIdx < MAX_LOGGER_NUM)
        {
            if (m_pLogger[loggerIdx])
            {
                //already existed!
                //if (m_pLogg)
                if (0 == strcmp(pName,m_pLogger[loggerIdx]->GetFileName()))
                {
                    return loggerIdx;
                }
                else
                {
                    printf("Recreate logger with different name!");
                    return ALREADY_EXIST;
                }
            }
        }
        else
        {
            return OUT_OF_RANGE;
        }
        
    }
    //走到这儿肯定有空位
    CLogger *pLogger = new CLogger();
    TInt32 ret = pLogger->Init(pName,logLvlWriteFileMask,logLvlPrintScreenMask,cacheLen);
    if (ret < SUCCESS)
    {
        delete pLogger;
        //init failed
        return OUT_OF_MEM;
    }
    m_pLogger[loggerIdx] = pLogger;
    return loggerIdx;
}

void CLoggerManager::ReleaseLogger(IfLogger *pLogger)
{
    for(TInt32 i=0;i<MAX_LOGGER_NUM;i++)
    {
        if (m_pLogger[i] == pLogger)
        {
            delete m_pLogger[i];
            m_pLogger[i] = NULL;
            //return SUCCESS;
        }
    }
    //return NOT_INITIALIZED;
}


void CLoggerManager::OnException(FILE *pDump)
{
    if (pDump)
    {
        char dumpBegin[] = "Begin to dump LoggerManger!";
        dumpBegin[sizeof(dumpBegin)-1] = '\n';
        fwrite(dumpBegin, sizeof(dumpBegin),1,pDump);
        fflush(pDump);
    }
    for (TInt32 i = 0;i<MAX_LOGGER_NUM;i++)
    {
        if (m_pLogger[i])
        {
            m_pLogger[i]->OnException(pDump);
        }
    }
    if (pDump)
    {
        char dumpSucces[] = "LoggerManger has dumped successful !";
        dumpSucces[sizeof(dumpSucces)-1] = '\n';
        fwrite(dumpSucces, sizeof(dumpSucces),1,pDump);
        fflush(pDump);
    }
    printf("LoggerManger has dumped!");
}

TInt32 CLoggerManager::Run(const TInt32 threadId,const TInt32 runCnt)
{
    TInt32 usedCnt = 0;
    
    time_t rawTime;
    struct tm * pTimeInfo;

    time ( &rawTime );

    pTimeInfo = localtime ( &rawTime );
    
    for (TInt32 i = 0;i<MAX_LOGGER_NUM;i++)
    {
        if (m_pLogger[i])
        {
            usedCnt += m_pLogger[i]->Run(runCnt,pTimeInfo);
        }
    }
    #ifdef _DEBUG
    if (usedCnt)
    {
        Sleep(15);
        usedCnt ++;
    }
    #endif
    return usedCnt;
}

void CLoggerManager::WriteLog(const TUInt32 loggerIdx,const TUInt32 logId,const TUInt32 logLvl,const TChar *__pFormat,...) 
{
    if (loggerIdx < MAX_LOGGER_NUM)
    {
        
    }
}

}

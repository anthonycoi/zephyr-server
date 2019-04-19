#ifndef __ZEPHYR_SYSTEM_LOGGER_LOGGER_H__
#define __ZEPHYR_SYSTEM_LOGGER_LOGGER_H__


#include <stdio.h>
#include "../../include/SysInc.h"
#include "../../../Public/include/Pipe.h"
#include "../../../Public/include/TypeDef.h"
#include "../../include/Lock.h"
#include "../../../Public/Interface/Platform/include/IfLogger.h"

#include <time.h>

#define MAX_LOGGER_FILE_NAME_LENGTH (32)
#define MAX_LOGGER_LENGTH (2048)
#define MAX_LOGGER_CONTENTS_LENGTH (2000)


#define MIN_FILE_FFLUSH_SIZE (4*1024)
#define FORCED_FFLUSH_LOOP_NR 10

#define MAX_LOGGER_FILE_SIZE (100 * 1024 * 1024)

namespace Zephyr
{


class CLogger : public IfLogger
{
protected:
    CPipe   m_pipe;
    TChar   m_fileName[MAX_LOGGER_FILE_NAME_LENGTH];
    TChar   m_buff[MAX_LOGGER_LENGTH];
    
    //record the day
    //to flush all the thing every 1 sec, no matter the length of available length

    //TUInt16 m_fileCreateDay;
    TUInt32 m_fileWriteLength;
    TUInt32 m_cacheCriticalLen;
    
    TUInt32 m_printToScreen;
    TUInt32 m_writeToFile;
    TUInt32 m_needOperation;
    
    struct tm m_timeNow;
    
    //Increase every time!
    TUInt32 m_idleLoopTimes;
    TUInt32 m_loggerCount;
    FILE*    m_file;
    #ifdef _DEBUG
    volatile TUInt32 m_exceptionAccured;  
    #endif
private:
    //write thread lock this, when it is full , trys to lock this, the work threads will unlock it!
    CLock   m_lock;
public:
    CLogger();
    ~CLogger();
    TInt32 Init(const TChar *pName,TUInt32 logLvlWriteFileMask = 0xFFFFFFFF,TUInt32 logLvlPrintScreenMask = 0,TUInt32 cacheLen = DEFAULT_CACHED_LOG_LENGTH);
    TInt32 CreateFile();
    void ChangeWriteFileLvl(TUInt32 newLvlMask);
    void ChangePrintScreenLvl(TUInt32 newLvlMask);
    //必须记录日志号
    virtual void WriteLog(const TUInt32 logId,const TUInt32 lvl,const TChar *_pFormat,...);
    virtual void WriteLog(const TUInt32 lvl,const TChar *_pFormat,...);
    virtual void WriteLog(const TUInt32 logId,const TUInt32 lvl,const TChar *_pFormat,va_list ValueList);
    virtual void WriteLog(const TUInt32 lvl,const TChar *_pFormat,va_list ValueList);
    virtual void WriteRawLog(const TUInt32 lvl,const TChar *__pFormat,...);
    virtual void WriteBinLog(const TChar *pBin,TUInt32 uLength);
    TChar *GetFileName()
    {
        return m_fileName;
    }
private:
    void AppRun(TInt32 neededLen);
    
public:
//do not call this !
    TInt32 Run(TUInt32 runCnt,struct tm *pTime);
    void OnDateChanged();
    TBOOL NeedUnlock();
    void OnException(FILE *pDump = NULL);
};




}


#endif

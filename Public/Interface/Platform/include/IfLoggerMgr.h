#ifndef __ZEPHYR_SYSTEM_LOGGER_IF_LOGGER_MGR_H_
#define __ZEPHYR_SYSTEM_LOGGER_IF_LOGGER_MGR_H_
#include "TypeDef.h"
#include "IfLogger.h"
#include "IfFinalizer.h"
namespace Zephyr
{


class IfLoggerManager
{
public:
    virtual IfFinalizer *GetLogFinalizer() = 0;
    //如果loggerIdx 为-1,则由系统来分配一个.
    virtual TInt32 AddLogger(const TChar *pName,TInt32 loggerIdx,TUInt32 logLvlWriteFileMask = 0xFFFFFFFF,TUInt32 logLvlPrintScreenMask = 0,TUInt32 cacheLen = DEFAULT_CACHED_LOG_LENGTH) = 0;
    virtual void ReleaseLogger(IfLogger *pLogger) = 0;
    virtual void WriteLog(const TUInt32 loggerIdx,const TUInt32 logId,const TUInt32 logLvl,const TChar *__pFormat,...) = 0;

    virtual IfLogger *GetLogger(TInt32 idx) = 0;
};



#define DECLA

#define LOG(LogId,Level,Format,...)  \
    CIfLogerMgr::WriteLog();

}


#endif

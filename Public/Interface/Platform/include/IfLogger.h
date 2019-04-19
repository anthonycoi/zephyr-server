#ifndef __ZEPHYR_SYSTEM_LOGGER_IF_LOGGER_H__
#define __ZEPHYR_SYSTEM_LOGGER_IF_LOGGER_H__
#include "stdarg.h"
#include "../../../include/TypeDef.h"

namespace Zephyr
{
#define DEFAULT_CACHED_LOG_LENGTH (1024 * 1024)

enum EnLogLvl
{
    log_critical     = 1,               //紧急，运行时的致命错误，比如内存不够、网络断开
    log_run          = (1<<1),          //运行日志，比如物品创建、任务创建等
    log_test         = (1<<2),          //测试模式
    log_debug        = (1<<3),          //debug日志，在release版中不存在.
    log_release_mode = (log_critical|log_run),
    log_test_mode    = (log_critical|log_run|log_test),
    log_debug_mode   = (log_critical|log_run|log_test|log_debug),
};

#ifdef  _DEBUG
#define LOG_WRITE_MODE log_debug_mode 
#define LOG_PRINT_SCREEN_MODE log_test 
#else
#define LOG_WRITE_MODE log_release_mode
#define LOG_PRINT_SCREEN_MODE log_critical
#endif



class IfLogger
{
public:
    virtual void WriteLog(const TUInt32 logId,const TUInt32 lvl,const TChar *_pFormat,...) = 0;
    virtual void WriteLog(const TUInt32 lvl,const TChar *_pFormat,...) = 0;
    virtual void WriteLog(const TUInt32 logId,const TUInt32 lvl,const TChar* __pFormat,va_list vl) = 0;
    virtual void WriteLog(const TUInt32 lvl,const TChar *__pFormat,va_list vl) = 0;
    //直接写数据，不按格式写
    virtual void WriteRawLog(const TUInt32 lvl,const TChar *__pFormat,...) = 0;

    //直接写比特流，不要随便用.
    virtual void WriteBinLog(const TChar *pBin,TUInt32 uLength) = 0;
};

//这里的Format不能是char*指针，必须是字符串"",就能自动带入File和Line
#define LOG_DEBUG(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_debug,"[File:%s,Line:%u]"##Format,__FILE__,__LINE__,__VA_ARGS__)

//这里的Format不能是char*指针，必须是字符串"",就能自动带入File和Line
#define LOG_CRITICAL(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_critical,"[File:%s,Line:%u]"##Format,__FILE__,__LINE__,__VA_ARGS__)

//每一个运行日志都必须有自己的日志号，这样，可以根据日志号，直接找到错误点.
#define LOG_RUN(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_run,Format,__VA_ARGS__)

//这里的Format不能是char*指针，必须是字符串"",就能自动带入File和Line
#define LOG_TEST(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_test,"[File:%s,Line:%u]"##Format,__FILE__,__LINE__,__VA_ARGS__)




}


#endif

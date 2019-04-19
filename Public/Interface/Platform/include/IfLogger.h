#ifndef __ZEPHYR_SYSTEM_LOGGER_IF_LOGGER_H__
#define __ZEPHYR_SYSTEM_LOGGER_IF_LOGGER_H__
#include "stdarg.h"
#include "../../../include/TypeDef.h"

namespace Zephyr
{
#define DEFAULT_CACHED_LOG_LENGTH (1024 * 1024)

enum EnLogLvl
{
    log_critical     = 1,               //����������ʱ���������󣬱����ڴ治��������Ͽ�
    log_run          = (1<<1),          //������־��������Ʒ���������񴴽���
    log_test         = (1<<2),          //����ģʽ
    log_debug        = (1<<3),          //debug��־����release���в�����.
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
    //ֱ��д���ݣ�������ʽд
    virtual void WriteRawLog(const TUInt32 lvl,const TChar *__pFormat,...) = 0;

    //ֱ��д����������Ҫ�����.
    virtual void WriteBinLog(const TChar *pBin,TUInt32 uLength) = 0;
};

//�����Format������char*ָ�룬�������ַ���"",�����Զ�����File��Line
#define LOG_DEBUG(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_debug,"[File:%s,Line:%u]"##Format,__FILE__,__LINE__,__VA_ARGS__)

//�����Format������char*ָ�룬�������ַ���"",�����Զ�����File��Line
#define LOG_CRITICAL(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_critical,"[File:%s,Line:%u]"##Format,__FILE__,__LINE__,__VA_ARGS__)

//ÿһ��������־���������Լ�����־�ţ����������Ը�����־�ţ�ֱ���ҵ������.
#define LOG_RUN(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_run,Format,__VA_ARGS__)

//�����Format������char*ָ�룬�������ַ���"",�����Զ�����File��Line
#define LOG_TEST(LogId,Format,...)	GetLogger()->WriteLog(LogId,log_test,"[File:%s,Line:%u]"##Format,__FILE__,__LINE__,__VA_ARGS__)




}


#endif

#include "Logger.h"
#include "SysMacros.h"


#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif


 

namespace Zephyr
{


CLogger::CLogger()
{
    memset(m_fileName,0,sizeof(m_fileName));
    memset(m_buff,0,sizeof(m_buff));
    time_t rawTime;
    struct tm * pTimeInfo;

    time ( &rawTime );

    pTimeInfo = localtime ( &rawTime );
    m_timeNow = *pTimeInfo;
    //m_lastWriteTime = 0;
    m_idleLoopTimes = 0;
    //m_fileCreateDay = 0;
    m_fileWriteLength = 0;
    m_printToScreen = 0;
    m_writeToFile = 0;
    m_cacheCriticalLen = 0;
    m_needOperation = 0;
    m_file = NULL;
#ifdef _DEBUG
    m_exceptionAccured = 0;  
#endif
}

CLogger::~CLogger()
{
    TInt32 dataLen = m_pipe.GetDataLen();
    m_fileName[sizeof(m_fileName)-1] = '\0';
    m_lock.Lock();
    while (dataLen>0)
    {

        TUInt32 writeSize = dataLen;

        TUChar *pData = m_pipe.GetData(writeSize);
        if (m_file)
        {
            fwrite(pData,writeSize,1,m_file);
            fflush(m_file);
        }
        else
        {

        }
        m_pipe.ReturnMsgBuff(pData,writeSize);
        dataLen = m_pipe.GetDataLen();
    }
    m_lock.Unlock();
}

TInt32 CLogger::Init(const TChar *pName,TUInt32 logLvlWriteFileMask /*= 0xFFFFFFFF*/,TUInt32 logLvlPrintScreenMask /*= 0*/,TUInt32 cacheLen/* = DEFAULT_CACHED_LOG_LENGTH*/)
{
    //TChar fullName[128];
    TInt32 ret = m_pipe.Init(cacheLen);
    if (ret < SUCCESS)
    {
        return ret;
    }
    
    strncpy(m_fileName,pName,(sizeof(m_fileName)-1));
    m_fileName[(sizeof(m_fileName)-1)] = '\0';
    m_printToScreen = logLvlPrintScreenMask;
    m_writeToFile = logLvlWriteFileMask;
    m_needOperation = m_printToScreen | m_writeToFile;
    //sprintf(fullName,"log\\%s_%d_%d_%d_%d.log",pName,m_timeMonth,m_timeDay,m_timeHour,m_timeMinute);
    //m_file = fopen(pName,"a+");
    m_cacheCriticalLen = (cacheLen>>3) + MAX_LOGGER_LENGTH;
    return CreateFile();
}

TInt32 CLogger::CreateFile()
{
    //if (m_file)
    TChar fullName[128];
    //only to minuts
    
    
    time_t rawTime;
    struct tm * timeInfo;

    time ( &rawTime );

    timeInfo = localtime ( &rawTime );

    //m_fileCreateDay = timeInfo->tm_yday;
    
    TInt32 month = timeInfo->tm_mon;
    TInt32 day   = timeInfo->tm_mday;
    TInt32 hour  = timeInfo->tm_hour;
    TInt32 min   = timeInfo->tm_min;
    
#ifdef _WIN32
    if (!CreateDirectory ("Log", NULL))
    {
        //printf("CreateDir Log failed!");
    }
#else

    int status;
    status = mkdir("/Log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


#endif
    
    sprintf(fullName,"Log\\%s_%d_%d_%d_%d_%d.log",m_fileName,month,day,hour,min,timeInfo->tm_sec);
    FILE *pNew = fopen(fullName,"a+");
    m_lock.Lock();
    if (m_file)
    {
        fclose(m_file);
    }
    m_file = pNew;
    m_lock.Unlock();
    if (!m_file)
    {
        printf("Open log file %s failed!",fullName);
        return FAIL;
    }
    else
    {
        m_fileWriteLength = 0;
    }
    return SUCCESS;
}

void CLogger::ChangeWriteFileLvl(TUInt32 newLvlMask)
{
    m_writeToFile = newLvlMask;
    m_needOperation = m_printToScreen | m_writeToFile;
}

void CLogger::ChangePrintScreenLvl(TUInt32 newLvlMask)
{
    m_printToScreen = newLvlMask;
    m_needOperation = m_printToScreen | m_writeToFile;
}

inline void CLogger::AppRun(TInt32 needLength)
{
    //the writer will lock this lock, when it finish writing the datas,
    //it will unlock this lock, the blocked user will rescheduled to continue the process
    
    TUInt32 freeLength = m_pipe.GetFreeLen();
    //check again
    TUInt32 writenSize = 0;
    if (freeLength < needLength)
    {
        m_lock.Lock();
        
        TUInt32 freeLen = m_pipe.GetFreeLen();
        TUInt32 dataLen = m_pipe.GetDataLen();
        while(freeLen < needLength)
        {
            TUInt32 writeSize;
            if (dataLen > MIN_FILE_FFLUSH_SIZE)
            {
                writeSize = MIN_FILE_FFLUSH_SIZE;
            }
            else
            {
                writeSize = dataLen;
            }
            //TUChar *pBuff = new TUChar[MIN_FILE_FFLUSH_SIZE];
            TUInt32 availableSize = writeSize;
            TUChar *p = m_pipe.GetData(availableSize);
            if (m_file)
            {
                fwrite(p,availableSize,1,m_file);
                writenSize += availableSize;
                fflush(m_file);
            }
            

            dataLen -= availableSize;

            m_pipe.ReturnMsgBuff(p,availableSize);
            freeLen = m_pipe.GetFreeLen();
        }
        m_fileWriteLength += writenSize;
        m_lock.Unlock();
    }
    
}




//do not call this !
TInt32 CLogger::Run(TUInt32 runCnt,struct tm *pTimeNow)
{
    m_timeNow = *pTimeNow;
    
    TUInt32 dataLen = m_pipe.GetDataLen();
    
    //no logs
    if (0 == dataLen)
    {
        return 0;
    }
    if (dataLen < MIN_FILE_FFLUSH_SIZE)
    {
        m_idleLoopTimes ++;
        if (m_idleLoopTimes < FORCED_FFLUSH_LOOP_NR)
        {
            return 0;
        }
    }

    TUInt32 freeLen = m_pipe.GetFreeLen();
    //
    TUInt32 writedBytes = 0;
    
    while(freeLen < m_cacheCriticalLen)
    {
        m_lock.Lock();
        
        TUInt32 writeSize;
        if (dataLen > MIN_FILE_FFLUSH_SIZE)
        {
            writeSize = MIN_FILE_FFLUSH_SIZE;
        }
        else
        {
            writeSize = dataLen;
        }
        //TUChar *pBuff = new TUChar[MIN_FILE_FFLUSH_SIZE];
        TUInt32 availableSize = writeSize;
        TUChar *p = m_pipe.GetData(availableSize);
        if (!p)
        {
            //printf("something wrong!");
            //availableSize = 0;
        }
        else
        {
            if (m_file)
            {
                fwrite(p,availableSize,1,m_file);
                fflush(m_file);
            }
            m_fileWriteLength += availableSize;
            //dataLen -= availableSize;

            m_pipe.ReturnMsgBuff(p,availableSize);
        }
        
        
        
        freeLen = m_pipe.GetFreeLen();

        m_lock.Unlock();
    }
    
            
    //else just fflush them all.
    //m_fileWriteLength += dataLen;
    m_lock.Lock();
    dataLen = m_pipe.GetDataLen();
    while (dataLen>0)
    {
        
        TUInt32 writeSize = dataLen;
        
        TUChar *pData = m_pipe.GetData(writeSize);
        if (pData)
        {
            if (m_file)
            {
                fwrite(pData,writeSize,1,m_file);
                fflush(m_file);
            }
            m_fileWriteLength += writeSize;
            m_pipe.ReturnMsgBuff(pData,writeSize);
        }
        dataLen = m_pipe.GetDataLen();
#ifdef _DEBUG
        if(m_exceptionAccured)
        {
            printf("Exception accured, quit!");
            break;
        }  
#endif
    }
    m_lock.Unlock();
    if (m_fileWriteLength > MAX_LOGGER_FILE_SIZE)
    {
        CreateFile();
    }
    return 1;
}



void CLogger::OnDateChanged()
{
    CreateFile();
}

void CLogger::OnException(FILE *pDump /*= NULL*/)
{
    TInt32 tryTimes = 0;
    //
    printf("Exception go Sleep");
    Sleep(2000);
    
    TInt32 dataLen = m_pipe.GetDataLen();
    m_fileName[sizeof(m_fileName)-1] = '\0';
    while (dataLen>0)
    {

        TUInt32 writeSize = dataLen;

        TUChar *pData = m_pipe.GetData(writeSize);
        if (m_file)
        {
            fwrite(pData,writeSize,1,m_file);
            fflush(m_file);
        }
        else
        {
            
            if (pDump)
            {
                TChar buff[256];
                TInt32 len = sprintf(buff,"Logger %s is corrupted!\n",m_fileName);
                fwrite(buff,len,1,pDump);
            }
            
        }
        m_pipe.ReturnMsgBuff(pData,writeSize);
        dataLen = m_pipe.GetDataLen();
    }
    if (pDump)
    {
        TChar buff[256];
        TInt32 len = sprintf(buff,"Logger %s has dumped!\n",m_fileName);
        fwrite(buff,len,1,pDump);
    }
}



void CLogger::WriteLog(const TUInt32 logId,const TUInt32 lvl,const TChar *_pFormat,...)
{
    va_list argList;
    va_start(argList,_pFormat);
    WriteLog(logId,lvl,_pFormat,argList);
    va_end(argList);
}


void CLogger::WriteLog(const TUInt32 logId,const TUInt32 lvl,const TChar *_pFormat,va_list ValueList)
{
    if (!(lvl & m_needOperation))
    {
        return;
    }
    m_loggerCount ++;

    TChar *pBuff;
    TUInt32 maxLogLenth = MAX_LOGGER_LENGTH;
    pBuff = (TChar *)m_pipe.PrepareMsg(maxLogLenth);
    if (maxLogLenth < MAX_LOGGER_LENGTH)
    {
        pBuff = m_buff;
    }


    TInt32 strLen = sprintf(pBuff,"[%d][%02d-%02d %02d-%02d-%02d]",
        logId,(m_timeNow.tm_mon+1),m_timeNow.tm_mday,m_timeNow.tm_hour,m_timeNow.tm_min,m_timeNow.tm_sec); 

    TInt32 strLen2 = _vsnprintf((pBuff+strLen),MAX_LOGGER_CONTENTS_LENGTH,_pFormat,ValueList);

    if (strLen2 <= 0)
    {
        return;
    }
    strLen += strLen2;
    //add and '\n' to the end.
    if ('\n' != pBuff[(strLen-1)])
    {
        // MAX_LOGGER_LENGTH - MAX_LOGGER_CONTENTS_LENGTH == 48, enough for a lot of 
        pBuff[strLen] = '\n';
        strLen ++;
    }


    //TInt32 strLen = pEnd - m_buff;
    if (lvl & m_writeToFile)
    {
        if (pBuff == m_buff)
        {
            TInt32 ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
            while(ret < 0)
            {
                //app will write the file to HD;
                AppRun(strLen);
                ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
            }
        }
        else // already write into pipe!
        {
            m_pipe.ConfirmAddMsg((TUChar*)pBuff,strLen);
        }
    }

    if (lvl & m_printToScreen)
    {
        pBuff[strLen] = '\0';
        printf("%s",m_buff);
    }
}


void CLogger::WriteLog(const TUInt32 lvl,const TChar *_pFormat,...)
{
    va_list argList;
    va_start(argList,_pFormat);
    WriteLog(lvl,_pFormat,argList);
    va_end(argList);
}

void CLogger::WriteLog(const TUInt32 lvl,const TChar *_pFormat,va_list ValueList)
{
    if (!(lvl & m_needOperation))
    {
        return;
    }
    m_loggerCount ++;

    TChar *pBuff;
    TUInt32 maxLogLenth = MAX_LOGGER_LENGTH;
    pBuff = (TChar *)m_pipe.PrepareMsg(maxLogLenth);
    if (maxLogLenth < MAX_LOGGER_LENGTH)
    {
        pBuff = m_buff;
    }

    TInt32 strLen = sprintf(pBuff,"[%02d-%02d %02d-%02d-%02d]",
        (m_timeNow.tm_mon+1),m_timeNow.tm_mday,m_timeNow.tm_hour,m_timeNow.tm_min,m_timeNow.tm_sec); 

    TInt32 strLen2 = _vsnprintf((pBuff+strLen),MAX_LOGGER_CONTENTS_LENGTH,_pFormat,ValueList);

    if (strLen2 <= 0)
    {
        return;
    }
    strLen += strLen2;
    //add and '\n' to the end.
    if ('\n' != pBuff[(strLen-1)])
    {
        // MAX_LOGGER_LENGTH - MAX_LOGGER_CONTENTS_LENGTH == 48, enough for a lot of thing
        pBuff[strLen] = '\n';
        strLen ++;
    }


    //TInt32 strLen = pEnd - m_buff;
    if (lvl & m_writeToFile)
    {
        if (pBuff == m_buff)
        {
            TInt32 ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
            while(ret < 0)
            {
                //app will write the file to HD;
                AppRun(strLen);
                ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
            }
        }
        else // already write into pipe!
        {
            m_pipe.ConfirmAddMsg((TUChar*)pBuff,strLen);
        }
    }

    if (lvl & m_printToScreen)
    {
         pBuff[strLen] = '\0';
        printf("%s",m_buff);
    }
}

void CLogger::WriteRawLog(const TUInt32 lvl,const TChar *__pFormat,...)
{
    if (!(lvl & m_needOperation))
    {
        return;
    }
    m_loggerCount ++;

    TChar *pBuff;
    TUInt32 maxLogLenth = MAX_LOGGER_LENGTH;
    pBuff = (TChar *)m_pipe.PrepareMsg(maxLogLenth);
    if (maxLogLenth < MAX_LOGGER_LENGTH)
    {
        pBuff = m_buff;
    }
    va_list argList;
    va_start(argList,__pFormat);
    
    TInt32 strLen = _vsnprintf((pBuff),MAX_LOGGER_CONTENTS_LENGTH,__pFormat,argList);
    va_end(argList);
    if (strLen <= 0)
    {
        return;
    }
    //不加任何东西
//     if ('\n' != pBuff[(strLen-1)])
//     {
//         // MAX_LOGGER_LENGTH - MAX_LOGGER_CONTENTS_LENGTH == 48, enough for a lot of thing
//         pBuff[strLen] = '\n';
//         strLen ++;
//     }


    //TInt32 strLen = pEnd - m_buff;
    if (lvl & m_writeToFile)
    {
        if (pBuff == m_buff)
        {
            TInt32 ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
            while(ret < 0)
            {
                //app will write the file to HD;
                AppRun(strLen);
                ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
            }
        }
        else // already write into pipe!
        {
            m_pipe.ConfirmAddMsg((TUChar*)pBuff,strLen);
        }
    }

    if (lvl & m_printToScreen)
    {
         pBuff[strLen] = '\0';
        printf("%s",m_buff);
    }
}

void CLogger::WriteBinLog(const TChar *pBin,TUInt32 uLength)
{
    m_loggerCount ++;

    TChar *pBuff;
    TUInt32 maxLogLenth = uLength + 64;
    pBuff = (TChar *)m_pipe.PrepareMsg(maxLogLenth);
    if (maxLogLenth < MAX_LOGGER_LENGTH)
    {
        pBuff = m_buff;
    }
    TUInt32 strLen = sprintf_s(pBuff,maxLogLenth,"Length:%u,[Start]",uLength);
    memcpy(pBuff+strLen,pBin,uLength);
    strLen += uLength;
    //长度其实可以不计
    strLen += sprintf_s(pBuff+strLen,maxLogLenth-strLen,"[End]");
    if (pBuff == m_buff)
    {
        TInt32 ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
        while(ret < 0)
        {
            //app will write the file to HD;
            AppRun(strLen);
            ret = m_pipe.WriteData((TUChar*)m_buff,strLen);
        }
    }
    else // already write into pipe!
    {
        m_pipe.ConfirmAddMsg((TUChar*)pBuff,strLen);
    }
}

}

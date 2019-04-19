#ifndef  __ZEPHYR_PUBLIC_STATISTIC_STATISTIC_H__
#define __ZEPHYR_PUBLIC_STATISTIC_STATISTIC_H__


#include "SysMacros.h"
#include "TypeDef.h"
#include "string.h"

namespace Zephyr
{

#define MAX_STACK_NR (128)


_declspec(naked) __int64 RTSC()
{
    __asm
    {
        rdtsc
            ret
    }
}
//
//TUInt32 CPUID()
//{
//    TUInt32 tmp;
//    __asm
//    {
//        //pushad
//        //mov   eax,0
//        //cpuid
//        //MOV tmp, EBX//将EBX中的值放到dwCPUName中
//        //popad
//    }
//}
class TStatisticData
{
private:
    TUInt64 m_timeUsed;
    TUInt64 m_timesCalled;
public:
    TStatisticData()
    {
        m_timeUsed = 0;
        m_timesCalled = 0;
    }
    void AddUsedTime(TUInt64 time)
    {
        m_timesCalled ++;
        m_timeUsed += time;
    }

};



template<TUInt32 FuncNr>
class CStatisticMgr
{
private:
    static CStatisticMgr *sm_pInstance;
    CStatisticMgr()
    {
        OnInit();
    }
    //for static methods;
public:
    static CStatisticMgr *Instance()
    {
        return sm_pInstance;
    }
    
    //for class methods
public:
    void OnInit()
    {
        memset(m_statistics,0,sizeof(CStatisticMgr));
        
        
    }
    TInt32 MakeRecord()
    {
    
    }
public:

    void PushStack(TUInt32 stackIdx)
    {
        m_stackNow ++;
        if (m_stackNow <MAX_STACK_NR)
        {
            m_stackIdx[m_stackNow] = stackIdx;
        }
        if (m_stackNow > m_maxStackNr)
        {
            //record the max stack trace.
            m_maxStackNr = m_stackNow;
            memcpy(m_maxStackTrace,m_stackIdx,(m_stackNow * sizeof(TUInt32)));
        }
    }
    void PopStack(TUInt64 timeUsed,TUInt32 funcIdx)
    {
        m_statistics[funcIdx][(FuncNr-1)].AddUsedTime(timeUsed);
        m_stackNow --;
        m_statistics[m_stackIdx[m_stackNow]][funcIdx].AddUsedTime(timeUsed);
        
    }
private:
    TStatisticData     m_statistics[FuncNr][FuncNr];
    //need lock?
    TUInt32     m_lastRecordNr;
    TUInt32     m_stackNow;
    TUInt32     m_maxStackNr;
    TUInt32     m_stackIdx[MAX_STACK_NR];
    TUInt32     m_maxStackTrace[MAX_STACK_NR];
    //FILE        m_recordFiles;
public:
    class CStatistic
    {
    public:
        TInt16      m_funcIdx;
        TInt16      m_needLock;
        TUInt32     m_cpuId;
        TInt64      m_timeBegin;
        CStatistic(TInt32 funcIdx)
        {
            m_funcIdx = funcIdx;
            m_timeBegin = RTSC();
            //CPUID(m_cpuId);
            CStatisticMgr::Instance()->PushStack(m_funcIdx);
        }
        ~CStatistic()
        {
            TUInt32 cpuidNow;
           /* CPUID(cpuidNow);*/
            TInt64 timeUsed = RTSC();
            timeUsed -= m_timeBegin;
            /*
            if (cpuidNow == m_cpuId)
            {
                timeUsed = RTSC();
                timeUsed -= m_timeBegin;
            }
            else
            {
                timeUsed = 0;
            }*/
            
            if (timeUsed < 0)
            {
                timeUsed = 0;
            }
            CStatisticMgr::Instance()->PopStack(timeUsed,m_funcIdx);
        }
    };
};


#ifdef __NEED_STATISTIC__

#define ON_ENTER_CLASS_METHOD(class_name,method_name) CStatisticMgr<(enum_max_statistic_func_nr)>::CStatistic tmpStatistic(enum_statistic_##class_name##_##method_name##_idx);

#define ON_ENTER_FUNC(funcName) CStatisticMgr<(enum_max_statistic_func_nr)>::CStatistic tmpStatistic(enum_statistic_system_##funcName##_idx);

#define DECLAIR_TO_USE_STATISTIC \
    CStatisticMgr<(enum_max_statistic_func_nr)> *CStatisticMgr<(enum_max_statistic_func_nr)>::sm_pInstance = new CStatisticMgr<(enum_max_statistic_func_nr)>();\
    


#define BEGIN_DEFINE_ALL_FUNC \
enum EnAllStatisticFunc\
{\

#define ADD_STATISTIC_FUNC(name) enum_statistic_system_##name##_idx,
#define ADD_STATISTIC_CLASS_METHOD(class_name,method_name) enum_statistic_##class_name##_##method_name##_idx,

#define END_ADD_STATISTIC_ALL_FUNC enum_max_statistic_func_nr \
}; 

#else

#define ON_ENTER_CLASS_METHOD(class_name,method_name)

#define ON_ENTER_FUNC(funcName) 

#define DECLAIR_TO_USE_STATISTIC \
  



#define BEGIN_DEFINE_ALL_FUNC \


#define ADD_STATISTIC_FUNC(name) 
#define ADD_STATISTIC_CLASS_METHOD(class_name,method_name) 

#define END_ADD_STATISTIC_ALL_FUNC 

#endif

}

#endif

#include "../include/Clock.h"

#ifdef _WIN32
//#include  <mmsystem.h>
#include <windows.h>

#else
#include<sys/time.h>
#endif

namespace Zephyr 
{

CClock::CClock()
{
    m_timeIdx = 0;
#ifdef _WIN32
    m_timeNow = (TUInt32)timeGetTime();
#else
    timeval * tv;
    gettimeofday(tv, 0 );
    m_timeNow = tv->tv_usec + (tv->tv_sec * 1000);
#endif
    for (int i=0;i<4;++i)
    {
        m_nPlatformTime[i] = 0;
    }
}
    
void CClock::Update()
{
#ifdef _WIN32
    TUInt32 timeNow = (TUInt32)timeGetTime();
#else
    timeval * tv;
    gettimeofday(tv, 0 );
    TUInt32 timeNow = (tv->tv_usec/1000) + (tv->tv_sec * 1000);
#endif
    int gap;
    if (timeNow >= m_timeNow) //这儿不用存，因为就是这儿更新的
    {
        gap = timeNow - m_timeNow;
    }
    else
    {
        gap = (((TUInt32)0xFFFFFFFF) - m_timeNow) + timeNow;
    }
    m_timeNow = timeNow;
    TUInt64 nT = m_nPlatformTime[m_timeIdx] + gap;
    m_nPlatformTime[(m_timeIdx&0x00000003)] = nT;
    TUInt32 nNew = m_timeIdx + 1;
    
    if (nNew >=4)
    {
        nNew = 0; 
    }
    m_timeIdx = nNew;
}

//解决64位数读取问题.
TUInt64 CClock::GetPlatformTime() const
{
    TUInt32 nIdx = m_timeIdx;
    TUInt64 uT = m_nPlatformTime[(nIdx&0x00000003)];
    TUInt32 nIdxNow = m_timeIdx;
    if (nIdxNow != nIdx)
    {
        TUInt32 nGap;
        if (nIdxNow >= nIdx)
        {
            nGap = (nIdxNow - nIdx);
        }
        else
        {
            nGap = ((((TUInt32)0xFFFFFFFF) - nIdx) + nIdxNow);
        }
        while (nGap > 1)
        {
            nIdx = m_timeIdx;
            uT = m_nPlatformTime[(nIdx&0x00000003)];
            nIdxNow = m_timeIdx;

            if (nIdxNow >= nIdx)
            {
                nGap = (nIdxNow - nIdx);
            }
            else
            {
                nGap = ((((TUInt32)0xFFFFFFFF) - nIdx) + nIdxNow);
            }
        }
    }
    return uT;
}
}

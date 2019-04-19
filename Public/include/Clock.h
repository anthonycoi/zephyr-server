#ifndef __ZEPHYR_PUBLIC_CLOCK_H__
#define __ZEPHYR_PUBLIC_CLOCK_H__

#include "TypeDef.h"


namespace Zephyr
{

class CClock
{
private:
    volatile TUInt64 m_nPlatformTime[4];
    
public:
    volatile TUInt32 m_timeIdx; //ϵͳ����ʱ��
    volatile TUInt32 m_timeNow;
    //�����ȫƽ̨��ͳһʱ�䣬��Comm��ͬ��������˵�������Ǹ�Comm�ϲ㣬ֻҪʹ��Comm�����ܻ����ͬ��m_nPlatformTime,ϣ���ܾ�ȷ��40ms����
    
    
public:
    CClock();
    TUInt32 GetLocalTime() const
    {
        return m_timeNow;
    }
    //ע�⣬nLast�������ڱ��ε���֮ǰ��õģ����صĿ϶�������.
    TUInt32 GetTimeGap(TUInt32 nLast) const
    {
        TUInt32 timeNow = m_timeNow; //һ��Ҫ�ȴ�����
        if (timeNow >= nLast)
        {
            return  timeNow - nLast;
        }
        else
        {
            return  ((((TUInt32)0xFFFFFFFF) - nLast) + timeNow);
        }
    }
    TUInt64 GetPlatformTime() const;
    void Update();
    
};



}
#endif

/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTimer.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 定时器接口
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef   __ZEPHYR_IF_TIMER_H__
#define   __ZEPHYR_IF_TIMER_H__

#include "../../../include/TypeDef.h"
#include "../../App/include/IfScheduler.h"
namespace Zephyr
{

class IfTimer
{
public:
    //设置定时器，nGap是定的时常,IfScheduler是回调接口,注意如果uGapInMs为0，则会立刻重复调用nRepeatTime次，如果nRepeatTime为-1，则表示永远循环
    //这就会死循环！
    virtual void *SetTimer(TUInt32 uGapInMs,TInt32 nRepeatTime,IfScheduler *pScheduler,TUInt64 nTimeNow) = 0;

    //删除定时器pTimer是SetTimer返回的结果
    virtual IfScheduler *KillTimer(void *pTimer) = 0;

    //Run是要定时去调用的，然后产生回调.
    virtual TUInt32 Run(TUInt64 nTimeNow) = 0;
};

}

#endif

/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTimer.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ��ʱ���ӿ�
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
    //���ö�ʱ����nGap�Ƕ���ʱ��,IfScheduler�ǻص��ӿ�,ע�����uGapInMsΪ0����������ظ�����nRepeatTime�Σ����nRepeatTimeΪ-1�����ʾ��Զѭ��
    //��ͻ���ѭ����
    virtual void *SetTimer(TUInt32 uGapInMs,TInt32 nRepeatTime,IfScheduler *pScheduler,TUInt64 nTimeNow) = 0;

    //ɾ����ʱ��pTimer��SetTimer���صĽ��
    virtual IfScheduler *KillTimer(void *pTimer) = 0;

    //Run��Ҫ��ʱȥ���õģ�Ȼ������ص�.
    virtual TUInt32 Run(TUInt64 nTimeNow) = 0;
};

}

#endif

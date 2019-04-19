/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfScheduler.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ��ʱ���ص��ӿ�
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_IF_SCHEDULER_H__
#define __ZEPHYR_IF_SCHEDULER_H__

#include "../../../include/TypeDef.h"

namespace Zephyr
{


class IfScheduler
{
public:
    virtual TInt32 OnScheduler(void *pTimer,TUInt64 nTimeNow) = 0;
};

}

#endif

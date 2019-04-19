/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  ����ӿ�
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_TASK_H__
#define __ZEPHYR_IF_TASK_H__


 
#include "../../../include/TypeDef.h"
#include "../../../include/SysMacros.h"
namespace Zephyr
{
    
enum EnTaskPriority
{
    insignificant_task      = 0x00,          //������task��ø��ٵ�runCnt
    normal_task             = 0x01,         //��ѯ����
    very_urgent_task        = 0x02,         //ÿ����CPU �ͻ�������task,����һֱ����֪�����߳����¿���
};

class IfTask
{
public:
    virtual TInt32 Begin(TInt32 threadId) = 0;
    virtual TInt32 Run(const TInt32 threadId,const TInt32 runCnt) = 0;
    virtual TInt32 End(TInt32 threadId)    = 0;
    virtual ~IfTask()
    {

    }
public:
    //void    TrySleep(unsigned int ms);
};

}
#endif

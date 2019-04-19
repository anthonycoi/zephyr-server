/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTaskMgr.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  任务管理器接口
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_TASK_MGR_H__
#define __ZEPHYR_IF_TASK_MGR_H__
#include "Typedef.h"
#include "IfTask.h"




namespace Zephyr
{
    
class IfTaskMgr
{
public:
    //几个工作线程,nCpuNr不会对实际使用的cpu产生影响，但会对Task的调度产生影响，
    //即nCpuNr如果很小，在一个Worker完成一个任务后，会根据cpu数量去决定是否唤醒沉睡的其它worker线程
	virtual TInt32 StartWorking(TInt32 nrOfWorkersNeeded,TInt32 nCpuNr) = 0;
    //增加一个工作，最好在startworking前调用
    virtual TInt32 AddTask(IfTask *pTask,EnTaskPriority priority = normal_task) = 0;
    //减去一个工作，最好在startworking前调用
    virtual TInt32 ReleaseTask(IfTask *pTask) = 0;
    //停止工作
	virtual TInt32 StopWorking() = 0;
};

}


#endif

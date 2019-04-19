/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTaskMgr.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  ����������ӿ�
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
    //���������߳�,nCpuNr�����ʵ��ʹ�õ�cpu����Ӱ�죬�����Task�ĵ��Ȳ���Ӱ�죬
    //��nCpuNr�����С����һ��Worker���һ������󣬻����cpu����ȥ�����Ƿ��ѳ�˯������worker�߳�
	virtual TInt32 StartWorking(TInt32 nrOfWorkersNeeded,TInt32 nCpuNr) = 0;
    //����һ�������������startworkingǰ����
    virtual TInt32 AddTask(IfTask *pTask,EnTaskPriority priority = normal_task) = 0;
    //��ȥһ�������������startworkingǰ����
    virtual TInt32 ReleaseTask(IfTask *pTask) = 0;
    //ֹͣ����
	virtual TInt32 StopWorking() = 0;
};

}


#endif

/*-------------------------------------------------------------
 Copyright (C)| 
 File: TaskMgr.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  U must use this file in all platform related code!
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_TASK_CENTER_H__
#define __ZEPHYR_TASK_CENTER_H__

#include "../Interface/Platform/include/IfTaskMgr.h"
namespace Zephyr
{

IfTaskMgr *CreateTaskMgr();
TInt32 ReleaseTaskMgr(IfTaskMgr *pMgr);

}

#endif

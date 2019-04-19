#ifndef __ZEPHYR_PUBLIC_COMM_CENTER_H__
#define __ZEPHYR_PUBLIC_COMM_CENTER_H__
#include "../Interface/Platform/include/IfCommunicator.h"
#include "../Interface/Platform/include/IfTaskMgr.h"
#include "../Interface/Platform/include/IfLoggerMgr.h"

namespace Zephyr
{

IfCommunicatorMgr *CreateCommMgr(TInt32 nrOfWorkerThread,IfTaskMgr *pTaskMgr,IfLoggerManager *pLoggerMgr,const TChar *pConfigName=NULL);
IfCommunicatorMgr *CreateCommMgrWithConfig(TInt32 nrOfWorkerThread,IfTaskMgr *pTaskMgr,IfLoggerManager *pLoggerMgr,const TChar *pConfigName=NULL);
}


#endif

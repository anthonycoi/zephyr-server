#ifndef __ZEPHYR_PUBLIC_LOG_CENTER_H__
#define __ZEPHYR_PUBLIC_LOG_CENTER_H__

#include "../Interface/Platform/include/IfLoggerMgr.h"
#include "../Interface/Platform/include/IfLogger.h"
#include "../Interface/Platform/include/IfTaskMgr.h"

namespace Zephyr
{

IfLoggerManager *CreateLogSys(IfTaskMgr *pTaskMgr);


}

#endif

#include "LogCenter.h"
#include "LoggerManager.h"

namespace Zephyr
{

IfLoggerManager *CreateLogSys(IfTaskMgr *pTaskMgr)
{
    TInt32 ret = CLoggerManager::InitInstance();
    if (ret < SUCCESS)
    {
        return NULL;
    }
    CLoggerManager *pLogMgr = CLoggerManager::Instance();
    pTaskMgr->AddTask(pLogMgr);
    return pLogMgr;
}

}

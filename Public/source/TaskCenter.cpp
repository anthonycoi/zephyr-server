#include "TypeDef.h"
#include "SysMacros.h"
#include "TaskCenter.h"
#include "TaskMgr.h"

namespace Zephyr
{

IfTaskMgr *CreateTaskMgr()
{
    CTaskMgr *pMgr = new CTaskMgr();
    return pMgr;
}

TInt32 ReleaseTaskMgr(IfTaskMgr *p)
{
    CTaskMgr *pMgr = dynamic_cast<CTaskMgr *>(p);
    if (pMgr)
    {
        SAFE_DELETE(pMgr);
        return SUCCESS;
    }
    //else
    return CAN_NOT_UPPER_CAST_TO_DERIVED_CLASS;
}

}


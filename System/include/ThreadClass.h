#ifndef __ZEPHYR_THREAD_CLASS_H__
#define __ZEPHYR_THREAD_CLASS_H__
#include "../../Public/include/TypeDef.h"
#include "Lock.h"
namespace Zephyr
{

class CThread
{
   volatile TInt32 m_nContinue;
   CLock  m_cStopped;
public:
    CThread()
    {
        m_nContinue = 0;
    }
    TInt32 Start();
    static void   Loop(void *pFunc);

    virtual TInt32 OnStart();
    virtual TInt32 Run(TInt32 runCun) = 0;
    virtual TInt32 OnStop();

    TInt32 Stop(TBOOL bBlock = true);
};

}
#endif

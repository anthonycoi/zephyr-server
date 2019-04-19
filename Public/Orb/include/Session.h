#ifndef __ZEPHYR_SERVICE_CONTAINER_SESSION_H__
#define __ZEPHYR_SERVICE_CONTAINER_SESSION_H__

#include "./Skeleton.h"
#include "../../include/TypeDef.h"
#include "../../include/SysMacros.h"
#include "./DistributedObject.h"

namespace Zephyr
{

class CService;

class CSession : public CObject
{
private:
    CService    *m_pService;
public:
    CSession();
    virtual ~CSession();
    //在创建的时候调用
    TInt32 Init(CService *pService)
    {
        m_pService = pService;
    }
};

}

#endif

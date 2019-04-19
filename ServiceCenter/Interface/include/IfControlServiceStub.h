#ifndef __IFCONTROLSERVICE_STUB_H__
#define __IFCONTROLSERVICE_STUB_H__
#include "Public/include/TypeDef.h"
#include "Public/include/Doid.h"
#include "Public/Interface/Platform/include/IfSkeleton.h"
#include "../Interface/IfControlService.h"
namespace Zephyr 
{
class IfControlServiceStub : public IfControlService
{
public:
    IfSkeleton *m_pOnwerObj;
    CDoid  m_tTarget;
    void Init(IfSkeleton *pSkeleton,CDoid *pDoid)
    {
        m_pOnwerObj = pSkeleton;
        m_tTarget = *pDoid;
    }
    virtual TInt32 StartService(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tConfigs);
    virtual TInt32 Command(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tCommands);
    virtual TInt32 StopService(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tDatas);
};
}
#endif

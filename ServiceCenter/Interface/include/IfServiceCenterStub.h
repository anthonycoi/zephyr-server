#ifndef __IFSERVICECENTER_STUB_H__
#define __IFSERVICECENTER_STUB_H__
#include "Public/include/TypeDef.h"
#include "Public/include/Doid.h"
#include "Public/Interface/Platform/include/IfSkeleton.h"
#include "../Interface/IfServiceCenter.h"
namespace Zephyr 
{
class IfServiceCenterStub : public IfServiceCenter
{
public:
    IfSkeleton *m_pOnwerObj;
    CDoid  m_tTarget;
    void Init(IfSkeleton *pSkeleton,CDoid *pDoid)
    {
        m_pOnwerObj = pSkeleton;
        m_tTarget = *pDoid;
    }
    TInt32 OnSerivceStarted(TUInt32 nServiceId,const TChar* pszName,CDoid tDoid);
    TInt32 OnServiceStoped(TUInt32 nServiceId,CDoid tDoid);
    TInt32 ServiceStatic(TUInt32 uServiceId,CDoid tDoid,TLV<TUInt16,TUInt16> tDatas);
};
}
#endif

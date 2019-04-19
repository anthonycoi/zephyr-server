#ifndef __IFCONNECTING_STUB_H__
#define __IFCONNECTING_STUB_H__
#include "Public/include/TypeDef.h"
#include "Public/include/Doid.h"
#include "Public/Interface/Platform/include/IfSkeleton.h"
#include "../Interface/IfConnecting.h"
namespace Zephyr 
{
class IfConnectingStub : public IfConnecting
{
public:
    IfSkeleton *m_pOnwerObj;
    CDoid  m_tTarget;
    void Init(IfSkeleton *pSkeleton,CDoid *pDoid)
    {
        m_pOnwerObj = pSkeleton;
        m_tTarget = *pDoid;
    }
    virtual TInt32 RegisterService(CDoid* pDoid,TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd,TUInt32 uPriority);
    virtual TInt32 UnregisterService(TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd);
    virtual TInt32 RegisterTeam(TUInt32 uTeamID);
    virtual TInt32 Disconnect(TUInt32 uReason);
    virtual TInt32 SetId(TUInt32 uId);
    virtual TInt32 CheckId(TUInt32 uId);
};
}
#endif

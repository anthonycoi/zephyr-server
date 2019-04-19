#ifndef __ZEPHYR_GATEWAY_ROUTE_MAP_H__
#define __ZEPHYR_GATEWAY_ROUTE_MAP_H__
#include "Route.h"
#include "Public/tpl/include/TplMultiMap.h"
#include "Public/tpl/include/TplPool.h"
#include "Public/Interface/Platform/include/IfLogger.h"
#include "Public/include/Doid.h"

namespace Zephyr
{

class CRouteMap
{
private:
    TplMultiKeyMap<CRoute,TUInt32>             m_tServiceRoute;

public:
    CRouteMap()
    {
    }
    void Init(CPool<TplMultiKeyMapNode<CRoute,TUInt32> > *pPool)
    {
        m_tServiceRoute.Init(pPool);
    }
    //×ó¿ªÓÒ±Õ
    TInt32 AddRoute(CDoid *pDoid,TUInt32 uSrvId,TUInt32 uBegin,TUInt32 uEnd,TUInt32 uPriority);
    //×ó¿ªÓÒ±Õ
    TInt32 RmvRoute(CDoid *pDoid,TUInt32 uSrvId,TUInt32 uBegin,TUInt32 uEnd);
    TInt32 ChangePriorty(TUInt32 uServiceId,CDoid *pMyDoid,TUInt32 uPriority);
    CDoid *FindService(TUInt32);
    void OnFinal();
};

}
#endif

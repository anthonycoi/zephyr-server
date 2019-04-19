#ifndef __ZEPHYR_SERVICE_CENTER_IF_CONTROL_SERVICE_H__
#define __ZEPHYR_SERVICE_CENTER_IF_CONTROL_SERVICE_H__

#include "Public/include/Typedef.h"

namespace Zephyr
{

class IfServiceCenter
{
public:
    virtual TInt32 OnSerivceStarted(TUInt32 nServiceId,const TChar* pszName,CDoid tDoid) = 0;
    virtual TInt32 OnServiceStoped(TUInt32 nServiceId,CDoid tDoid) = 0;
    virtual TInt32 ServiceStatic(TUInt32 uServiceId,CDoid tDoid,TLV<TUInt16,TUInt16> tDatas) = 0;
    virtual TInt32 HeartBeat(TUInt32 uServiceId,CDoid) = 0;
};

}

#endif

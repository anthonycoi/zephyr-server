#ifndef __ZEPHYR_SERVICE_CENTER_IF_CONTROL_SERVICE_H__
#define __ZEPHYR_SERVICE_CENTER_IF_CONTROL_SERVICE_H__

#include "Public/include/Typedef.h"

namespace Zephyr
{

class IfControlService
{
public:
    virtual TInt32 StartService(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tConfigs) = 0;

    virtual TInt32 Command(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tCommands) = 0;

    virtual TInt32 StopService(TUInt32 uServiceId,TLV<TUInt32,TUInt32> tDatas) = 0;
};

}

#endif

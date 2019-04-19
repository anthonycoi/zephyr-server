#ifndef __ZEPHYR_SERVICE_CENTER_H__
#define __ZEPHYR_SERVICE_CENTER_H__
#include "../Interface/interface/IfServiceCenter.h"
#include "ServiceData.h"
namespace zephyr
{

class CServiceCenter : public CService,
                       public IfServiceCenter
{
private:


public:

    DECALRE_HANDLE_INTERFCE;

    virtual TInt32 OnSerivceStarted(TUInt32 nServiceId,const TChar* pszName,CDoid tDoid);
    virtual TInt32 OnServiceStoped(TUInt32 nServiceId,CDoid tDoid);
    virtual TInt32 ServiceStatic(TUInt32 uServiceId,CDoid tDoid,TLV<TUInt16,TUInt16> tDatas);
    virtual TInt32 HeartBeat(TUInt32 uServiceId,CDoid);

};

}

#endif

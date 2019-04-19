#ifndef __ZEPHYR_SERVICE_CENTER_IF_UI__
#define __ZEPHYR_SERVICE_CENTER_IF_UI__

#include "ServiceData.h"


namespace Zephyr
{

class IfUI
{
public:
    void ShowOnePoint(CServiceData *pData,CDoid *pServiceDoid,IfServiceCallback *pCallback) = 0;
    void OnNewDependcyAvailable(TInt32 nDepencyIdx,CDoid *pTo) = 0;
};

class IfDataCenter
{
public:
    //获取某个节点的Service信息
    CServiceData *GetServiceData(CDoid *pDoid) = 0;
    //获取某种service数量，即其Doid.
    TInt32        GetServiceNr(CServiceName *pName,std::vector<CDoid> &tRtn) = 0;
};

class IfServiceCallback
{
public:
    TInt32 StartService(TChar *pParam1,TChar *pParam2,TChar *pParam3,TChar *pParam4) = 0;
    TInt32 Command(TChar *pParam1,TChar *pParam2,TChar *pParam3,TChar *pParam4) = 0;
    TInt32 StopService();
};


}



#endif

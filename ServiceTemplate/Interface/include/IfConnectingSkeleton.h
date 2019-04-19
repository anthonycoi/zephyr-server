#ifndef __IFCONNECTING_SKELETON_H__
#define __IFCONNECTING_SKELETON_H__
#include "Public/include/Message.h"
#include "../Interface/IfConnecting.h"
#include "IfConnectingMethodId.h"
namespace Zephyr 
{
class IfConnectingSkeleton 
{
public:
    IfConnecting *m_pImplementObj;
    IfConnectingSkeleton(IfConnecting *pIfObj)
    {
        m_pImplementObj = pIfObj;
    }
    TInt32 HandleMsg(CMessageHeader *pMsg);
    static TBOOL  IsMine(CMessageHeader *pMsg) //是否属于这个接口
     {
         return ((IFCONNECTING_INTERFACE_ID)&(pMsg->GetMethodId()));
     }
    TInt32 HandleRegisterService_CDoid_pt_TUInt32_TUInt32_TUInt32_TUInt32(CMessageHeader *pMsg);;
    TInt32 HandleUnregisterService_TUInt32_TUInt32_TUInt32(CMessageHeader *pMsg);;
    TInt32 HandleRegisterTeam_TUInt32(CMessageHeader *pMsg);;
    TInt32 HandleDisconnect_TUInt32(CMessageHeader *pMsg);;
    TInt32 HandleSetId_TUInt32(CMessageHeader *pMsg);;
    TInt32 HandleCheckId_TUInt32(CMessageHeader *pMsg);;
};
}
#endif

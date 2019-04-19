#ifndef __IFGATEWAYSVC_SKELETON_H__
#define __IFGATEWAYSVC_SKELETON_H__
#include "Public/include/Message.h"
#include "../Interface/IfGateway.h"
#include "IfGatewaySvcMethodId.h"
namespace Zephyr 
{
class IfGatewaySvcSkeleton 
{
public:
    IfGatewaySvc *m_pImplementObj;
    IfGatewaySvcSkeleton(IfGatewaySvc *pIfObj)
    {
        m_pImplementObj = pIfObj;
    }
    TInt32 HandleMsg(CMessageHeader *pMsg);
    static TBOOL  IsMine(CMessageHeader *pMsg) //是否属于这个接口
     {
         return ((IFGATEWAYSVC_INTERFACE_ID)&(pMsg->GetMethodId()));
     }
    TInt32 HandleSyn2Map_TUInt32_TLV_tpl_begin_TUInt8_and_TUInt16_tpl_end__ref(CMessageHeader *pMsg);;
    TInt32 HandleRegisterService_TUInt32_TUInt32_TUInt32_TUInt32_OctSeq_tpl_begin_TUInt16_tpl_end__ref(CMessageHeader *pMsg);;
    TInt32 HandleUnRegisterService_TUInt32_TUInt32_TUInt32(CMessageHeader *pMsg);;
    TInt32 HandleBroadcastTeamMsg_TUInt32_OctSeq_tpl_begin_TUInt16_tpl_end__ref(CMessageHeader *pMsg);;
    TInt32 HandleChangePriorty_TUInt32_CDoid_pt_TUInt32(CMessageHeader *pMsg);;
    TInt32 HandleStartLogin_TUInt32_TUInt16_TUInt16(CMessageHeader *pMsg);;
    TInt32 HandleStopLogin(CMessageHeader *pMsg);;
    TInt32 HandleDisconnectedAllClient(CMessageHeader *pMsg);;
};
}
#endif

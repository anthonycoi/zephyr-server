#ifndef __IFCONTROLSERVICE_SKELETON_H__
#define __IFCONTROLSERVICE_SKELETON_H__
#include "Public/include/Message.h"
#include "../Interface/IfControlService.h"
#include "IfControlServiceMethodId.h"
namespace Zephyr 
{
class IfControlServiceSkeleton 
{
public:
    IfControlService *m_pImplementObj;
    IfControlServiceSkeleton(IfControlService *pIfObj)
    {
        m_pImplementObj = pIfObj;
    }
    TInt32 HandleMsg(CMessageHeader *pMsg);
    static TBOOL  IsMine(CMessageHeader *pMsg) //是否属于这个接口
     {
         return ((IFCONTROLSERVICE_INTERFACE_ID)&(pMsg->GetMethodId()));
     }
    TInt32 HandleStartService_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_(CMessageHeader *pMsg);
    TInt32 HandleCommand_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_(CMessageHeader *pMsg);
    TInt32 HandleStopService_TUInt32_TLV_tpl_begin_TUInt32_and_TUInt32_tpl_end_(CMessageHeader *pMsg);
};
}
#endif

#ifndef __IFSERVICECENTER_SKELETON_H__
#define __IFSERVICECENTER_SKELETON_H__
#include "Public/include/Message.h"
#include "../Interface/IfServiceCenter.h"
#include "IfServiceCenterMethodId.h"
namespace Zephyr 
{
class IfServiceCenterSkeleton 
{
public:
    IfServiceCenter *m_pImplementObj;
    IfServiceCenterSkeleton(IfServiceCenter *pIfObj)
    {
        m_pImplementObj = pIfObj;
    }
    TInt32 HandleMsg(CMessageHeader *pMsg);
    static TBOOL  IsMine(CMessageHeader *pMsg) //是否属于这个接口
     {
         return ((IFSERVICECENTER_INTERFACE_ID)&(pMsg->GetMethodId()));
     }
    TInt32 HandleOnSerivceStarted_TUInt32_const_TChar_pt_CDoid(CMessageHeader *pMsg);;
    TInt32 HandleOnServiceStoped_TUInt32_CDoid(CMessageHeader *pMsg);;
    TInt32 HandleServiceStatic_TUInt32_CDoid_TLV_tpl_begin_TUInt16_and_TUInt16_tpl_end_(CMessageHeader *pMsg);;
};
}
#endif

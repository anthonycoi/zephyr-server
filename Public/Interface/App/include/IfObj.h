/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfObj.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ORB回调接口,与IfSkeleton相配对
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_SKELETON_H__
#define __ZEPHYR_IF_SKELETON_H__
#include "../../../include/Message.h"
#include "../../../include/TypeDef.h"
namespace Zephyr
{

class IfObj
{
public:
    //当收到消息的时候，回调这个,如果非0，则表示消息被处理
    virtual TInt32  OnRecv(CMessageHeader *pMsg) = 0;
    
//     //定时器到时了
//     virtual TInt32  OnTimer(TInt32 nTimerIdx,void *pData,TInt32 nTimeGap,TUInt32 nTimeNow) = 0;

    //以下是给Service使用的
    //定期的回调，可以注册循环时间，但只能有一个
    virtual TInt32  OnRoutine(TUInt32 nRunCnt) = 0;
    //网络时间
    virtual TInt32  OnNetEvent(CConnectionEvent *pEvent) = 0;

};

}

#endif

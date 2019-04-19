/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfSkeleton.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: IfSkeleton接口,有了这个一个对象就能发消息了,代表了对象在ORB的注册结果
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_STUB__
#define __ZEPHYR_IF_STUB__

#include "../../../include/Message.h"
#include "../../../include/Clock.h"
namespace Zephyr
{



class IfSkeleton
{
public:
    virtual CDoid *GetMyDoid() = 0;
    //获得后自己初始化
    virtual CMessageHeader *PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid* destDoid,TInt32 destDoidNum,bool bRearrangeDest)  = 0;
    //发送消息
    virtual TInt32  SendMsg(CMessageHeader *pMsg) = 0;

    //这是为Service准备的,每个comm就只能有一个.为服务器特殊设计.不要这个，直接到IfOrb去注册.
    //virtual TInt32 RegisterRun(TInt32 nRunGapInMs) = 0;
};

}

#endif

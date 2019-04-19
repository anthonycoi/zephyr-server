/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfOrb.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ORB接口,用以分配IfStub
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_ORB_H__
#define __ZEPHYR_IF_ORB_H__


#include "../../App/include/IfObj.h"
#include "IfSkeleton.h"
#include "IfCommunicator.h"

namespace Zephyr
{

class IfOrb
{
public:
    //注册Obj
    virtual IfSkeleton* RegisterObj(IfObj *pIfObj,TInt16 nSrvId) = 0;
    //注册特殊的ObjIdfx
    //注销
    virtual void    UnRegisterObj(IfSkeleton *pSkeleton) = 0;
    //运行时不能再注册Service
    //virtual IfSkeleton* RegiterService(IfObj *pObj,TInt16 nSrvId) = 0;
    //这是为Service准备的, 每个comm就只能有一个.为服务器特殊设计.
    virtual TInt32 RegisterRun(IfSkeleton *pObj,TUInt32 nGapInMs) = 0;
    //时间相关,获取一次就行了，这个不会变
    virtual const CClock *GetClock() = 0;

    virtual IfCommunicator *GetCommunicator() = 0;
//  这个放到应用层去做 
//     //查找Service
//     CDoid   FindService(TCHAR *pName) = 0;
//     CDoid   FindService(TUInt32 nServiceId) = 0;
    
    
};


}

#endif


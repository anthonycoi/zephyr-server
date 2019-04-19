#ifndef __ZEPHYR_IF_GATEWAY_H__
#define __ZEPHYR_IF_GATEWAY_H__

#include "Public/include/Typedef.h"

namespace Zephyr
{
class IfGatewaySvc
{
public:
    //同步消息.
    virtual TInt32 Syn2Map(TUInt32 uFrom,TLV<TUInt8,TUInt16>& tTLV) = 0;
    //注册服务 
    virtual TInt32 RegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd,TUInt32 uPriority,OctSeq<TUInt16>& tServiceName) = 0;
    //注销服务
    virtual TInt32 UnRegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd) = 0;
    //发送广播聊天信息
    virtual TInt32 BroadcastTeamMsg(TUInt32 uTeam,OctSeq<TUInt16>& tServiceName) = 0;

    virtual TInt32 ChangePriorty(TUInt32 uServiceId,CDoid *pMyDoid,TUInt32 uPriority) = 0;

    //开始接收登陆，供管理使用。主控服务器在所有内部服务器协调启动完成后，让gateway开始接收登陆.
    virtual TInt32  StartLogin(TUInt32 uIp,TUInt16 nListeningPort,TUInt16 nMaxConnection) = 0;
    //停止接入登陆，供管理使用。开始停服.这个时候Gateway只是停止接收新的连接，老连接还是维持的
    virtual TInt32  StopLogin() = 0;
    //关闭所有客户端连接
    virtual TInt32  DisconnectedAllClient() = 0;

};
}




#endif

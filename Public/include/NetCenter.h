#ifndef __ZEPHYR_PUBLIC_NET_CENTER_H__
#define __ZEPHYR_PUBLIC_NET_CENTER_H__
#include "../Interface/App/include/IfCryptorFactory.h"
#include "../Interface/App/include/IfParserFactory.h"
#include "../Interface/Platform/include/IfTaskMgr.h"
#include "TypeDef.h"
#include "../Interface/Platform/include/IfNet.h"
namespace Zephyr
{
    //pUser可以为空,如果非空，则每次收到新的消息，会去唤醒一下.
    //整个架构是net->comm->service，最多4个线程
    //service->comm,但是service不唤醒net,net阻塞在网络底层上1ms轮一次.
IfNet *CreateNet(IfTaskMgr *pTaskMgr,IfParserFactory *pParserFactory,IfCryptorFactory *pCryptorFactory=NULL,TInt32 maxConnectionNr=40,TUInt32 nSendBuffSize=256*1024,TUInt32 nRecvBuffSize=256*1024);
TInt32  DestoryNet(IfNet *pIfNet);
};

#endif;v
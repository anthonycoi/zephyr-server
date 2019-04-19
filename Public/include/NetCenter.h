#ifndef __ZEPHYR_PUBLIC_NET_CENTER_H__
#define __ZEPHYR_PUBLIC_NET_CENTER_H__
#include "../Interface/App/include/IfCryptorFactory.h"
#include "../Interface/App/include/IfParserFactory.h"
#include "../Interface/Platform/include/IfTaskMgr.h"
#include "TypeDef.h"
#include "../Interface/Platform/include/IfNet.h"
namespace Zephyr
{
    //pUser����Ϊ��,����ǿգ���ÿ���յ��µ���Ϣ����ȥ����һ��.
    //�����ܹ���net->comm->service�����4���߳�
    //service->comm,����service������net,net����������ײ���1ms��һ��.
IfNet *CreateNet(IfTaskMgr *pTaskMgr,IfParserFactory *pParserFactory,IfCryptorFactory *pCryptorFactory=NULL,TInt32 maxConnectionNr=40,TUInt32 nSendBuffSize=256*1024,TUInt32 nRecvBuffSize=256*1024);
TInt32  DestoryNet(IfNet *pIfNet);
};

#endif;v
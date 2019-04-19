#ifndef __ZEPHYR_PUBLIC_PLATFORM_IF_COMMUNICATOR_H__
#define __ZEPHYR_PUBLIC_PLATFORM_IF_COMMUNICATOR_H__

#include "../../../include/Message.h"
#include "../../../include/TypeDef.h"
#include "../../../include/Clock.h"
namespace Zephyr
{

#define MAX_SERVICE_NR_PER_COMM 8
#define MAX_COMM_NR (256/MAX_SERVICE_NR_PER_COMM)


class IfCommunicator
{
public:

    //
    virtual CMessageHeader *GetMsg(TUInt32 needRetMsgBuff = 1) = 0;

    virtual void ReturnMsgBuff(CMessageHeader *pMsg) = 0;

    virtual CMessageHeader *PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid srcId,CDoid* destDoid,TInt32 destDoidNum,bool bRearrangeDest) = 0;

    //virtual int SendMsg(SCTDMessageHeader *,bool bNeedCopy = false) = 0;
    //do not need the para bNeedCopy any more, I will check it!
    virtual TInt32 SendMsg(CMessageHeader *pMsg) = 0;

    //ע�������ʱ��ϵͳ
    virtual const CClock *GetClock() = 0;
// 
     virtual TInt32 GetNetEvent(CConnectionEvent &event) = 0;
    //maybe I could expose less header files
};

class IfCommunicatorMgr
{
public:
    //ע���ˣ�Comm�����𷵻ص�IfCommunicator���̰߳�ȫ����ʹ����Ҫ��֤ÿһ��IfCommunicatorֻ��һ���߳�ʹ��
    virtual TInt32          GetNrOfComm() = 0;
    virtual IfCommunicator *GetComm(TUInt16& nSrvBegin,TUInt16& nSrvEnd,TInt32 nCommIdx) = 0;
};

}


#endif

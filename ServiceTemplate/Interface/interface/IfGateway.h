#ifndef __ZEPHYR_IF_GATEWAY_H__
#define __ZEPHYR_IF_GATEWAY_H__

#include "Public/include/Typedef.h"

namespace Zephyr
{
class IfGatewaySvc
{
public:
    //ͬ����Ϣ.
    virtual TInt32 Syn2Map(TUInt32 uFrom,TLV<TUInt8,TUInt16>& tTLV) = 0;
    //ע����� 
    virtual TInt32 RegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd,TUInt32 uPriority,OctSeq<TUInt16>& tServiceName) = 0;
    //ע������
    virtual TInt32 UnRegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd) = 0;
    //���͹㲥������Ϣ
    virtual TInt32 BroadcastTeamMsg(TUInt32 uTeam,OctSeq<TUInt16>& tServiceName) = 0;

    virtual TInt32 ChangePriorty(TUInt32 uServiceId,CDoid *pMyDoid,TUInt32 uPriority) = 0;

    //��ʼ���յ�½��������ʹ�á����ط������������ڲ�������Э��������ɺ���gateway��ʼ���յ�½.
    virtual TInt32  StartLogin(TUInt32 uIp,TUInt16 nListeningPort,TUInt16 nMaxConnection) = 0;
    //ֹͣ�����½��������ʹ�á���ʼͣ��.���ʱ��Gatewayֻ��ֹͣ�����µ����ӣ������ӻ���ά�ֵ�
    virtual TInt32  StopLogin() = 0;
    //�ر����пͻ�������
    virtual TInt32  DisconnectedAllClient() = 0;

};
}




#endif

/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfNet.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  �����ӿ�
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_NET_H__
#define __ZEPHYR_IF_NET_H__

#include "../../../include/TypeDef.h"
#include "../../../include/SysMacros.h"
#include "IfNetData.h"

namespace Zephyr
{


enum EnConnectionState
{
    connection_is_not_in_use            = 0x00,
    connection_is_trying                = 0x01,
    connection_is_established           = 0x02,
    connection_is_using                 = 0x03,
    connection_is_broken                = 0x04,
    connection_is_aborted               = 0x05,
};

#define IF_NET_ERROR_CODE_OFFSET                        (-10000)
#define IF_NET_ERROR_CODE_TOO_MANY_PENDING_CONNECTIONGS (IF_NET_ERROR_CODE_OFFSET -1)

class IfConnection
{
public:
    //׼����Ϣ���������ô��
    //virtual TUChar *PrepareMsg(TUInt32 msgLen) = 0;
    //������Ϣ��ע�ⷢ�ͷ���0ֻ����Ϣ�ɹ����뻺�棬����һ�����շ��ͳɹ�
    virtual TInt32 SendMsg(TUChar *pMsg,TUInt32 msgLen) = 0;

    virtual TUInt32 GetFreeBuffLength() = 0;

    //��ȡ������Ϣ
    virtual CConPair *GetConnectionInfo() = 0 ;
    //�����Ƿ���ҪNegla�㷨
    virtual TInt32 NeedNoDelay(const char chOpt) = 0;
    //��ȡ����״̬.
    virtual EnConnectionState GetConnctionState() = 0;
    //���Ի�ȡ��δ���͵����ݵĳ���
    virtual TInt32 GetPendingDataLen() = 0;
    
    //��������󣬾Ϳ��Խ�IfConnectionCallBack�ͷ�.Net��������ص��ýӿ�.
    virtual TInt32 Disconnect() = 0;
};


class IfNet
{
public:
    //��������
    virtual TInt32 Connect(const TChar *pRemoteIp,const TChar *pMyIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack) = 0;
    
    virtual TInt32 Connect(const TUInt32 remoteIp,const TUInt32 myIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack) = 0;
    //virtual TInt32 Connect(TUInt32 remoteIp,TUInt32 myIp,TUInt16 remotePort,TUInt16 myPort,void *pAppCallBack) = 0;
        //sendRemainMsg �Ƿ���ʣ�����Ϣ
    
    //����
    virtual void *Listen(const TChar *pIp,TUInt16 port,TUInt16 maxConnection,void *pIfCallBack) = 0;

    virtual void *Listen(TUInt32 nIp,TUInt16 nPort,TUInt16 nMaxConnection,void *pIfCallBack) = 0;
    
    virtual TInt32 StopListening(void *pListener) = 0;
    
    //if chOpt = 1,disable the Nagle algrithom.
    //else enable it , it is enableed by default.
    //In general this is unnecessary.
    //����connection�ص�.
    virtual TInt32 Run(TUInt32 runCnt) = 0;
};


}
#endif

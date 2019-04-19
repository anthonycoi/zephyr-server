/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfNet.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  �����ص��ӿ�
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __IF_NET_APP_H__
#define __IF_NET_APP_H__

#include "../../../include/SysMacros.h"
#include "../../../include/TypeDef.h"
#include "IfParser.h"
#include "IfCryptor.h"
#include "../../Platform/include/IfNetData.h"
namespace Zephyr
{

enum EnEnableIf
{
    //����Ҫparser.
    en_if_connection_callback_on_connected_release_parser    = 0x01,
    //����Ҫcryptor.
    en_if_connection_callback_on_connected_release_cryptor   = 0x02,
};

class IfConnection;

class IfConnectionCallBack
{
public:
    //�յ���Ϣ��~
    virtual TInt32 OnRecv(TUChar *pMsg, TUInt32 msgLen) = 0;
    //virtual TInt32 OnRecvIn2Piece(TUChar *pMsg, TUInt32 msgLen,TUChar *pMsg2,TUInt32 msgLen2) = 0;
    //�������Զ���factory����parser��crypter,��Ӧ�ò��������������������
    //Ӧ�ò�Ӧ����ȷ֪��IfParser �� IfCryptor��ʵ���࣬����OnConnected��ʱ������������.
    //�������ֵen_if_connection_callback_on_connected_release_parser �����ˣ���ôparser�ᱻ�ͷ�
    //�������ֵen_if_connection_callback_on_connected_release_cryptor �����ˣ���ôpCryptor�ᱻ�ͷ�
    //IfConnection *pIfConnection������ʵ�ʽ�����ʱ���ٴ���Ӧ�ò㡣
    virtual TInt32 OnConnected(IfConnection *pIfConnection,IfParser *pParser,IfCryptor *pCryptor) = 0;
    
    //�κ�socket�쳣�����Զ��ر���������
    virtual TInt32 OnDissconneted(TInt32 erroCode) = 0;
};


class IfListenerCallBack
{
public:
    //���ؿգ����ͷ��������.
    //���callBack�ǵ���connectionMgr::run�󣬻ص��ģ�����ʵapp��һ���߳�,���õ����߳�����
    //���������,������connection�����ص�OnConnected���������Ӻͱ�������Ψһ��ͬ�ĵط�ʱ�����ڣ���������ʱ����������ʱ�����ṩIfConnectionCallBack
    //���������������ڳɹ�������Ӧ�ò�����IfConnectionCallBack��
    virtual IfConnectionCallBack *OnNewConnection(CConPair *pPair) = 0;
};



}










#endif

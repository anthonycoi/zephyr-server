/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfNet.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  传输层回调接口
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
    //不需要parser.
    en_if_connection_callback_on_connected_release_parser    = 0x01,
    //不需要cryptor.
    en_if_connection_callback_on_connected_release_cryptor   = 0x02,
};

class IfConnection;

class IfConnectionCallBack
{
public:
    //收到消息了~
    virtual TInt32 OnRecv(TUChar *pMsg, TUInt32 msgLen) = 0;
    //virtual TInt32 OnRecvIn2Piece(TUChar *pMsg, TUInt32 msgLen,TUChar *pMsg2,TUInt32 msgLen2) = 0;
    //网络层会自动从factory生成parser和crypter,请应用层对这连个东西进行设置
    //应用层应该明确知道IfParser 和 IfCryptor的实现类，并在OnConnected的时候对其进行设置.
    //如果返回值en_if_connection_callback_on_connected_release_parser 设置了，那么parser会被释放
    //如果返回值en_if_connection_callback_on_connected_release_cryptor 设置了，那么pCryptor会被释放
    //IfConnection *pIfConnection在连接实际建立的时候再传给应用层。
    virtual TInt32 OnConnected(IfConnection *pIfConnection,IfParser *pParser,IfCryptor *pCryptor) = 0;
    
    //任何socket异常都会自动关闭网络连接
    virtual TInt32 OnDissconneted(TInt32 erroCode) = 0;
};


class IfListenerCallBack
{
public:
    //返回空，则释放这个链接.
    //这个callBack是调用connectionMgr::run后，回调的，和其实app是一个线程,不用担心线程问题
    //调用完这个,还会由connection继续回调OnConnected，主动连接和被动连接唯一不同的地方时就在于，主动连接时在申请连接时主动提供IfConnectionCallBack
    //而被动连接则是在成功后，再向应用层申请IfConnectionCallBack。
    virtual IfConnectionCallBack *OnNewConnection(CConPair *pPair) = 0;
};



}










#endif

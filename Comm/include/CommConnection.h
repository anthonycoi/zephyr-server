/*-------------------------------------------------------------
 Copyright (C)| 
 File: CommConnection.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  传输层连接
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_COMM_COMM_CONNECTION_H__
#define __ZEPHYR_COMM_COMM_CONNECTION_H__

#include "IfNetApp.h"
#include "IfNet.h"

namespace Zephyr
{
class CCommMgr;

class CIpMapItem;

class CCommConnection : public IfConnectionCallBack
{
private:
    IfConnection*   m_pIfConnection;
    CIpMapItem      *m_pIpMapItem;
    TUInt32         m_usedTime;
    //#ifdef _DEBUG
    TUInt64         m_msgRecved;
    TUInt64         m_msgSend;
    
    TUInt16         m_nVirtualIp;
    TUInt16         m_nNodeId;
    
    CCommMgr        *m_pCommMgr;
    //#endif
     DECLARE_CLASS_LIST (CCommConnection)
public:
    CCommConnection();
    ~CCommConnection();
    TBOOL IsActived();
    TInt32 OnInit();
    TInt32 OnFinal();
    IfConnection *GetIfConnection()
    {
        return m_pIfConnection;
    }
    TInt32 Run();
    void SetAllInfo(CCommMgr *pMgr,CIpMapItem *pIp);
    TUInt16 GetNodeId()
    {
        return m_nNodeId;
    }
    TUInt16 GetVirtualIp()
    {
        return m_nVirtualIp;
    }

    virtual TInt32 OnRecv(TUChar *pMsg, TUInt32 msgLen);
    //virtual TInt32 OnRecvIn2Piece(TUChar *pMsg, TUInt32 msgLen,TUChar *pMsg2,TUInt32 msgLen2) = 0;
    //网络层会自动从factory生成parser和crypter,请应用层对这连个东西进行设置
    //应用层应该明确知道IfParser 和 IfCryptor的实现类，并在OnConnected的时候对其进行设置.
    //如果返回值en_if_connection_callback_on_connected_release_parser 设置了，那么parser会被释放
    //如果返回值en_if_connection_callback_on_connected_release_cryptor 设置了，那么pCryptor会被释放
    //IfConnection *pIfConnection在连接实际建立的时候再传给应用层。
    virtual TInt32 OnConnected(IfConnection *pIfConnection,IfParser *pParser,IfCryptor *pCryptor);

    //任何socket异常都会自动关闭网络连接
    virtual TInt32 OnDissconneted(TInt32 erroCode);

    //调用这个后，就可以将IfConnectionCallBack释放.Net不会继续回调该接口.
    TInt32 Disconnect();
};

}

#endif //__ZEPHYR_COMM_COMM_CONNECTION_H__

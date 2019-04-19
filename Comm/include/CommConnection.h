/*-------------------------------------------------------------
 Copyright (C)| 
 File: CommConnection.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  ���������
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
    //�������Զ���factory����parser��crypter,��Ӧ�ò��������������������
    //Ӧ�ò�Ӧ����ȷ֪��IfParser �� IfCryptor��ʵ���࣬����OnConnected��ʱ������������.
    //�������ֵen_if_connection_callback_on_connected_release_parser �����ˣ���ôparser�ᱻ�ͷ�
    //�������ֵen_if_connection_callback_on_connected_release_cryptor �����ˣ���ôpCryptor�ᱻ�ͷ�
    //IfConnection *pIfConnection������ʵ�ʽ�����ʱ���ٴ���Ӧ�ò㡣
    virtual TInt32 OnConnected(IfConnection *pIfConnection,IfParser *pParser,IfCryptor *pCryptor);

    //�κ�socket�쳣�����Զ��ر���������
    virtual TInt32 OnDissconneted(TInt32 erroCode);

    //��������󣬾Ϳ��Խ�IfConnectionCallBack�ͷ�.Net��������ص��ýӿ�.
    TInt32 Disconnect();
};

}

#endif //__ZEPHYR_COMM_COMM_CONNECTION_H__

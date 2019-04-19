#ifndef __ZEPHYR_COMM_WIN_NET_TEST_APP_CONNECTION_H__
#define __ZEPHYR_COMM_WIN_NET_TEST_APP_CONNECTION_H__

#include "IfNetApp.h"
#include "IfNet.h"

using namespace Zephyr;

class CAppConnectionMgr;
class CAppConnection : virtual public IfConnectionCallBack
{
private:
    IfConnection*   m_pIfConnection;
    CAppConnectionMgr   *m_pConnectionMgr;
    long long       m_msgRecved;
    long long       m_msgSend;
    int             m_passiveSendNr;
    CAppConnection* m_pNext;
    static int      sm_averageMsgLen;
    unsigned int    m_connectedTime;
    unsigned int    m_lastLogTime;
    
    unsigned int    m_unconnectedTime;
public:
    unsigned int    m_actived;
    void SetMgr(CAppConnectionMgr *pMgr)
    {
        m_pConnectionMgr = pMgr;
    }
    void OnTryConnection()
    {
        m_actived = 1;
    }
    CAppConnection();
    void SetPassvieSendNr(int nr)
    {
        m_passiveSendNr = nr;
    }
    TInt32 OnInit();
    TInt32 OnFinal();
    void Attach(CAppConnection *pNext)
    {
        m_pNext = pNext;
    }
    CAppConnection *Detach(CAppConnection *pPrev)
    {
        CAppConnection *pNext = m_pNext;
        if (pPrev)
        {
            pPrev->m_pNext = m_pNext;
        }
        m_pNext = NULL;
        return pNext;
    }
    
    TInt32 Run();
    static void SetAverageMsgLen(int len)
    {
        sm_averageMsgLen = len;
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
    
    
    //
    TInt32 SendMsg(TUChar *pMsg,TUInt32 msgLen) ;

    //获取连接信息
    CConPair *GetConnectionInfo();
    //设置是否需要Negla算法
    TInt32 NeedNoDelay(const char chOpt);
    //获取连接状态.
    EnConnectionState GetConnctionState();
    //用以获取还未发送的数据的长度
    TInt32 GetPendingDataLen();

    //调用这个后，就可以将IfConnectionCallBack释放.Net不会继续回调该接口.
    TInt32 Disconnect();    
};



#endif

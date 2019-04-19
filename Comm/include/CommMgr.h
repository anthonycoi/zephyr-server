#ifndef __ZEPHYR_COMM_MGR_H__
#define __ZEPHYR_COMM_MGR_H__
#include "TypeDef.h"
#include "IfCommunicator.h"
#include "Communicator.h"
#include "CommMacrosAndConsts.h"
#include "IfNet.h"
#include "IpMaps.h"
#include "TplPool.h"
#include "../Public/Interface/Platform/include/IfTaskMgr.h"
#include "CommConnection.h"
#include "MsgParserFactory.h"
#include "../../Public/include/Clock.h"
#include "IfLoggerMgr.h"
namespace Zephyr
{

enum EnCommOpr
{
    en_comm_id              = 0,
    en_comm_read_net_msg,
    en_comm_send_net_msg,
    en_comm_read_net_msg_blocked,
    en_comm_send_net_msg_blocked,
};
//添加一个IfConnection,这样处理本地消息的时候，也抽象成一个IfConnection.
/*处理群发消息逻辑：
尽量的省内存，如果是发给不同node的，则把所有发给该node的一起发，如果是同node,则按ip一起发，到了comm,则按同一srv一起发
最后再stub层进行obj层的重用.
发送不了就扔吧
*/
class CCommMgr : public IfCommunicatorMgr,public IfConnection, public IfListenerCallBack, public IfTask
{
private:
    TUInt32             m_nNrOfComm;
    CCommunicator       *m_pCommunicators;
    CIpMap              m_ipMaps;
    TUInt32             m_uLastCheckTime;
    IfNet               *m_pNet;
    CPool<CCommConnection> m_connectionPool;
    CMsgParserFactory   *m_pParserFactory;
    //CCommConnection     **m_ppConnections;

    EnCommOpr           m_enLastOpr;
    TUInt16             m_nNetBlockedOnIp;
    TUInt16             m_nAppBlockedOnSrv;
    TUInt32             m_netBlockTime;
    TUInt32             m_nAppBlockTime;

    CClock              m_tClock;
    TUChar              *m_pBuff;
    IfLoggerManager     *m_pLoggerMgr;
    IfLogger            *m_pLogger;
    CConPair             m_cLoopBack;
public:
    //taskMgr由ServerContainer生成.
    TInt32 Init(TInt32 nrOfWorkerThread,IfTaskMgr *pTaskMgr,IfLoggerManager *pIfLogMgr,const TChar *pConfigName=szDefaultCommConfigName);
    TInt32 InitWithConfig(TInt32 nrOfWorkerThread,IfTaskMgr *pTaskMgr,IfLoggerManager *pIfLogMgr,const TChar *pConfigStr);
    virtual TInt32          GetNrOfComm()
    {
        return m_nNrOfComm;
    }
    virtual IfCommunicator *GetComm(TUInt16& nSrvBegin,TUInt16& nSrvEnd,TInt32 nCommIdx);
    CClock *GetClock()
    {
        return &m_tClock;
    }
    virtual TInt32 Begin(TInt32 threadId)
    {
        return SUCCESS;
    }
    virtual TInt32 Run(const TInt32 threadId,const TInt32 runCnt);
    virtual TInt32 End(TInt32 threadId)
    {
        return SUCCESS;
    }
private:
    //返回值为是否需要丢弃消息,网络层阻塞了

    //返回值为是否需要丢弃消息,应用层阻塞了


    TBOOL IsAppStillBlocked();
    TBOOL IsNetStillBlocked();

    TBOOL CheckNetState(CMessageHeader *pMsg);
    void SendAppMsg(CMessageHeader *pMsg);

    void HandleOneNetMsg(CMessageHeader *pMsg);
    void HandleOneSystemMsg(CMessageHeader *pMsg);
    CCommunicator *GetIfComm(TUInt16 uSrvId)
    {
        //每个comm是16个service
        uSrvId /= MAX_SERVICE_NR_PER_COMM;
        if (uSrvId < m_nNrOfComm)
        {
            return m_pCommunicators + uSrvId;
        }
        return NULL;
    }
public:
    //发给本地.
    virtual TInt32 SendMsg(TUChar *pMsg,TUInt32 msgLen);
    //永远是最大
    virtual TUInt32 GetFreeBuffLength();
    //返回本地连接本地的
    //获取连接信息
    virtual CConPair *GetConnectionInfo() ;
    //设置是否需要Negla算法
    virtual TInt32 NeedNoDelay(const char chOpt);
    //获取连接状态.
    virtual EnConnectionState GetConnctionState();
    //用以获取还未发送的数据的长度
    virtual TInt32 GetPendingDataLen();

    //调用这个后，就可以将IfConnectionCallBack释放.Net不会继续回调该接口.
    virtual TInt32 Disconnect();

    //TInt32 DistributeSrvMsg(TInt32 idx);

public:
    virtual IfConnectionCallBack *OnNewConnection(CConPair *pPair);
    //获取虚信息.
    CIpMapItem *GetIpMapInfo(CConPair *pPair);

    void   OnConnected(CCommConnection *pConnection);
    void   OnDisconnected(CCommConnection *pConnection,TBOOL bIsNegative = TRUE);
};

}
#endif

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
//���һ��IfConnection,������������Ϣ��ʱ��Ҳ�����һ��IfConnection.
/*����Ⱥ����Ϣ�߼���
������ʡ�ڴ棬����Ƿ�����ͬnode�ģ�������з�����node��һ�𷢣������ͬnode,��ipһ�𷢣�����comm,��ͬһsrvһ��
�����stub�����obj�������.
���Ͳ��˾��Ӱ�
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
    //taskMgr��ServerContainer����.
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
    //����ֵΪ�Ƿ���Ҫ������Ϣ,�����������

    //����ֵΪ�Ƿ���Ҫ������Ϣ,Ӧ�ò�������


    TBOOL IsAppStillBlocked();
    TBOOL IsNetStillBlocked();

    TBOOL CheckNetState(CMessageHeader *pMsg);
    void SendAppMsg(CMessageHeader *pMsg);

    void HandleOneNetMsg(CMessageHeader *pMsg);
    void HandleOneSystemMsg(CMessageHeader *pMsg);
    CCommunicator *GetIfComm(TUInt16 uSrvId)
    {
        //ÿ��comm��16��service
        uSrvId /= MAX_SERVICE_NR_PER_COMM;
        if (uSrvId < m_nNrOfComm)
        {
            return m_pCommunicators + uSrvId;
        }
        return NULL;
    }
public:
    //��������.
    virtual TInt32 SendMsg(TUChar *pMsg,TUInt32 msgLen);
    //��Զ�����
    virtual TUInt32 GetFreeBuffLength();
    //���ر������ӱ��ص�
    //��ȡ������Ϣ
    virtual CConPair *GetConnectionInfo() ;
    //�����Ƿ���ҪNegla�㷨
    virtual TInt32 NeedNoDelay(const char chOpt);
    //��ȡ����״̬.
    virtual EnConnectionState GetConnctionState();
    //���Ի�ȡ��δ���͵����ݵĳ���
    virtual TInt32 GetPendingDataLen();

    //��������󣬾Ϳ��Խ�IfConnectionCallBack�ͷ�.Net��������ص��ýӿ�.
    virtual TInt32 Disconnect();

    //TInt32 DistributeSrvMsg(TInt32 idx);

public:
    virtual IfConnectionCallBack *OnNewConnection(CConPair *pPair);
    //��ȡ����Ϣ.
    CIpMapItem *GetIpMapInfo(CConPair *pPair);

    void   OnConnected(CCommConnection *pConnection);
    void   OnDisconnected(CCommConnection *pConnection,TBOOL bIsNegative = TRUE);
};

}
#endif

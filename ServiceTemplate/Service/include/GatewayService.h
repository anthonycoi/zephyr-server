#ifndef __ZEPHYR_GATEWAY_SERVICE_H__
#define __ZEPHYR_GATEWAY_SERVICE_H__

#include "Public/include/SysMacros.h"
#include "Public/Orb/include/Service.h"
#include "../../Interface/Interface/IfGateway.h"
#include "Public/include/IdlHeaderFile.h"
#include "Public/Interface/Platform/include/IfNet.h"
#include "Public/tpl/include/TplMultiMap.h"
#include "Public/tpl/include/TplPool.h"
#include "Route.h"
#include "Public/tpl/include/TplList.h"
#include "GatewaySession.h"
#include "./GatewayParserFactory.h"
#include "Public/include/TypeDef.h"
#include "GatewayLogger.h"
#include "RouteMap.h"

namespace Zephyr
{
class CGatewayService : public CService,
                        public IfListenerCallBack,
                        public IfGatewaySvc
{
private:
    IfNet   *m_pNet;
    //��OnInit��ʱ�򣬸���ServiceId������ȡLogger
    IfLoggerManager     *m_pLoggerManager;
    IfTaskMgr           *m_pTaskMgr;
    IfOrb               *m_pOrb;
    TUInt32 m_uIp;
    TUInt16 m_uListeningPort;
    TUInt16 m_nMaxConnections;
    void*   m_pListener;
    //һ��Service����һ��Pool������ʹ�þ�̬�����������Ļ�������ͬһServiceContainer�￪���Gateway.
    CPool<TplMultiKeyMapNode<CRoute,TUInt32> > m_tRoutePool;
    
    //
    CPool<CListNode<CGatewaySession> >         m_tSessionPool;

    CList<CGatewaySession>                     m_tUsingSessions;

    CRouteMap                                  m_tServiceRoute;

    IfLogger                                   *m_pLogger;

    CGatewayParserFactory                      m_tParserFactory;
    TUInt32                                    m_uLastRoutineTime;
public:
    CGatewayService();
    virtual IfConnectionCallBack *OnNewConnection(CConPair *pPair);

public:
    CPool<TplMultiKeyMapNode<CRoute,TUInt32> > *GetRoutePool()
    {
        return &m_tRoutePool;
    }
    DECALRE_HANDLE_INTERFCE;

    TInt32 InitService(IfOrb* pOrb,IfTaskMgr *pIfTaskMgr,IfLoggerManager *pIfLoggerMgr);
    //ͬ����Ϣ.
    virtual TInt32 Syn2Map(TUInt32 uFrom,TLV<TUInt8,TUInt16>& tTLV);
    //ע����� 
    virtual TInt32 RegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd,TUInt32 uPriority,OctSeq<TUInt16>& tServiceName);
    //ע������
    virtual TInt32 UnRegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd);
    //���͹㲥������Ϣ
    virtual TInt32 BroadcastTeamMsg(TUInt32 uTeam,OctSeq<TUInt16>& tServiceName);

    virtual TInt32 ChangePriorty(TUInt32 uServiceId,CDoid *pMyDoid,TUInt32 uPriority);

    //��ʼ���յ�½��������ʹ�á����ط������������ڲ�������Э��������ɺ���gateway��ʼ���յ�½.
    virtual TInt32  StartLogin(TUInt32 uIp,TUInt16 nListeningPort,TUInt16 nMaxConnection);
    //ֹͣ�����½��������ʹ�á���ʼͣ��.���ʱ��Gatewayֻ��ֹͣ�����µ����ӣ������ӻ���ά�ֵ�
    virtual TInt32  StopLogin();
    //�ر����пͻ�������
    virtual TInt32  DisconnectedAllClient();

    //������Serviceר�е�.
    virtual TInt32      OnInit();
    //�����ǻص�.
    virtual TInt32      OnFinal();


    //��ʱ  ����ʱ��
    virtual TInt32  OnTimer(TInt32 nTimerIdx,void *pData,TInt32 nTimeGap,TUInt32 nTimeNow);

    //�����Ǹ�Serviceʹ�õ�
    //���ڵĻص�������ע��ѭ��ʱ�䣬��ֻ����һ��
    virtual TInt32  OnRoutine(TUInt32 nRunCnt);
    //����ʱ��
    virtual TInt32  OnNetEvent(CConnectionEvent *pEvent);

    //��ʼ����

    //����Ĭ�ϵķ�����ڵ㣬����Щ������Ҫ��Ȩ��ע��Ҳ��ӵ�У������½
    CDoid *FindService(TUInt32);
    
    IfLogger *GetLogger()
    {
        return m_pLogger;
    }

    void OnDisconnected(CGatewaySession *pSession,IfParser *pParser,IfCryptor *pCryptor,TInt32 uReason);

private:
    TInt32 AddRoute(CDoid *pDoid,TUInt32 uSrvId,TUInt32 uBegin,TUInt32 uEnd,TUInt32 uPriority=0);
    
};



#ifdef WIN32

extern "C" __declspec( dllexport ) CService *InitService(IfOrb* pOrb,IfTaskMgr *pIfTaskMgr,IfLoggerManager *pIfLoggerMgr);

extern "C" __declspec( dllexport ) TInt32 ReleaseService(CService *);

#else

extern "C" CService *InitService(IfOrb* pStubCenter,IfTaskMgr *pIfTaskMgr,IfLoggerManager *pIfLoggerMgr);
extern "C" TInt32 ReleaseService(CService *);

#endif



}



#endif

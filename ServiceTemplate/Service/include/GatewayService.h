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
    //在OnInit的时候，根据ServiceId再来获取Logger
    IfLoggerManager     *m_pLoggerManager;
    IfTaskMgr           *m_pTaskMgr;
    IfOrb               *m_pOrb;
    TUInt32 m_uIp;
    TUInt16 m_uListeningPort;
    TUInt16 m_nMaxConnections;
    void*   m_pListener;
    //一个Service公用一个Pool，避免使用静态变量，这样的话，就在同一ServiceContainer里开多个Gateway.
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
    //同步消息.
    virtual TInt32 Syn2Map(TUInt32 uFrom,TLV<TUInt8,TUInt16>& tTLV);
    //注册服务 
    virtual TInt32 RegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd,TUInt32 uPriority,OctSeq<TUInt16>& tServiceName);
    //注销服务
    virtual TInt32 UnRegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd);
    //发送广播聊天信息
    virtual TInt32 BroadcastTeamMsg(TUInt32 uTeam,OctSeq<TUInt16>& tServiceName);

    virtual TInt32 ChangePriorty(TUInt32 uServiceId,CDoid *pMyDoid,TUInt32 uPriority);

    //开始接收登陆，供管理使用。主控服务器在所有内部服务器协调启动完成后，让gateway开始接收登陆.
    virtual TInt32  StartLogin(TUInt32 uIp,TUInt16 nListeningPort,TUInt16 nMaxConnection);
    //停止接入登陆，供管理使用。开始停服.这个时候Gateway只是停止接收新的连接，老连接还是维持的
    virtual TInt32  StopLogin();
    //关闭所有客户端连接
    virtual TInt32  DisconnectedAllClient();

    //以下是Service专有的.
    virtual TInt32      OnInit();
    //结束是回调.
    virtual TInt32      OnFinal();


    //定时  器到时了
    virtual TInt32  OnTimer(TInt32 nTimerIdx,void *pData,TInt32 nTimeGap,TUInt32 nTimeNow);

    //以下是给Service使用的
    //定期的回调，可以注册循环时间，但只能有一个
    virtual TInt32  OnRoutine(TUInt32 nRunCnt);
    //网络时间
    virtual TInt32  OnNetEvent(CConnectionEvent *pEvent);

    //开始接入

    //查找默认的服务入口点，即这些服务不需要鉴权和注册也能拥有，比如登陆
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

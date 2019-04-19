#include "../include/GatewayService.h"
#include "Public/include/SysMacros.h"
#include "../../Interface/include/IfGatewaySvcSkeleton.h"
#include "../include/GatewayBasicConfig.h"
#include "Public/include/NetCenter.h"
namespace Zephyr
{

IMPLEMENT_START_HANDLE_INTERFACE(CGatewayService)
IMPLEMENT_HANDLE_INTERFACE(IfGatewaySvc)
IMPLEMENT_END_HANDLE_INTERFACE(CGatewayService)


CGatewayService::CGatewayService()
{
    m_pNet = NULL;
    m_uIp  = NULL;
    m_uListeningPort = NULL;
    m_nMaxConnections;
    m_uLastRoutineTime = 0;
    m_pListener = NULL;
}

TInt32 CGatewayService::Syn2Map(TUInt32 uFrom,TLV<TUInt8,TUInt16>& tTLV)
{
    return SUCCESS;
}
//注册服务 
TInt32 CGatewayService::RegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd,TUInt32 uPriority,OctSeq<TUInt16>& tServiceName)
{
    AddRoute(GetCallerDoid(),uServiceId,uServicBegin,uEnd,uPriority);
    
    return SUCCESS;//表示处理了.
}
//注销服务
TInt32 CGatewayService::UnRegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd)
{
    CDoid *pRegister = GetCallerDoid();
    TInt32 nRet = m_tServiceRoute.RmvRoute(pRegister,uServiceId,uServicBegin,uEnd);
    if (nRet < SUCCESS)
    {
        char szBufferRegister[64];
        pRegister->ToStr(szBufferRegister);
        if (nRet < en_gw_error_begin)
        {
            LOG_RUN((-nRet),"UnRegister from:%s , uSrvId:%u,uBegin:%u,uEnd:%u,",szBufferRegister,uServiceId,uServicBegin,uEnd);
        }
        else
        {
            LOG_CRITICAL((-nRet),"UnRegister from :%s , uSrvId:%u,uBegin:%u,uEnd:%u,",szBufferRegister,uServiceId,uServicBegin,uEnd);
        }
    }
    return SUCCESS;
}
//发送广播聊天信息
TInt32 CGatewayService::BroadcastTeamMsg(TUInt32 uTeam,OctSeq<TUInt16>& tServiceName)
{
    return SUCCESS;
}

TInt32 CGatewayService::ChangePriorty(TUInt32 uServiceId,CDoid *pMyDoid,TUInt32 uPriority)
{
    TInt32 nRet = m_tServiceRoute.ChangePriorty(uServiceId,pMyDoid,uPriority);
    if (0 == nRet)
    {
        CDoid *pRegister = GetCallerDoid();
        char szBufferRegister[64];
        pRegister->ToStr(szBufferRegister);
        char szBufferDoid[64];
        pMyDoid->ToStr(szBufferDoid);
        LOG_RUN((-nRet),"Register:%s ,doid:%s, uSrvId:%u,uPriority:%u",szBufferRegister,szBufferDoid,uServiceId,uPriority);
    }
    return nRet;
}

//开始接收登陆，供管理使用。主控服务器在所有内部服务器协调启动完成后，让gateway开始接收登陆.
TInt32 CGatewayService::StartLogin(TUInt32 uIp,TUInt16 nListeningPort,TUInt16 nMaxConnection)
{
    if ((0 == m_uListeningPort)&&(0 == m_uIp))
    {
        IfListenerCallBack *pCallBack = this; //必须转一下，不位不对，Listen的参数void*,不是IfListenerCallBack*
        char szBuffer[64];
        GetCallerDoid()->ToStr(szBuffer);
        LOG_RUN(en_start_listing,"Caller(%s) start Listening at Ip:%u,port:%u,MaxConnection:%u",szBuffer,uIp,(TUInt32)nListeningPort,(TUInt32)nMaxConnection);
        
        m_pListener = m_pNet->Listen(uIp,nListeningPort,nMaxConnection,pCallBack);
        
        if (NULL == m_pListener)
        {
            LOG_RUN(en_start_listening_failed,"Listen failed! Ip:%u,port:%u",uIp,(TUInt32)nListeningPort);
            return -(en_start_listening_failed);
        }
    }
    else
    {
        //重复启动
        CDoid *pRegister = GetCallerDoid();
        char szBufferRegister[64];
        pRegister->ToStr(szBufferRegister);
        LOG_RUN(en_restart_listening,"Restart listening from doid:%s , New Ip:%u,port:%u,max connection:%u",szBufferRegister,uIp,(TUInt32)nListeningPort,(TUInt32)nMaxConnection);
    }
    return SUCCESS;
}
//停止接入登陆，供管理使用。开始停服.这个时候Gateway只是停止接收新的连接，老连接还是维持的
TInt32 CGatewayService::StopLogin()
{
    char szBuffer[64];
    GetCallerDoid()->ToStr(szBuffer);
    if (m_pListener)
    {
        LOG_RUN(en_stop_listening,"Caller(%s) stop Listening",szBuffer);
        m_pNet->StopListening(m_pListener); //肯定成功.
        m_pListener = NULL;
    }
    else
    {
        LOG_RUN(en_listening_not_started,"Caller(%s) try stop Listening,but there's no listening.",szBuffer);
    }
    return SUCCESS;
}
//关闭所有客户端连接
TInt32 CGatewayService::DisconnectedAllClient()
{
    return SUCCESS;
}

void CGatewayService::OnDisconnected(CGatewaySession *pSession,IfParser *pParser,IfCryptor *pCryptor,TInt32 uReason)
{
    //pParser \ pCryptor都不用释放.
    LOG_RUN(en_on_disconnected,"OnDisconnected,UserId:%llu,SystemId:%u,Reason%d",pSession->GetUserId(),pSession->GetSystemId(),uReason);

    m_pOrb->UnRegisterObj(pSession->GetSkeleton());
    CListNode<CGatewaySession> *pListNode = (CListNode<CGatewaySession> *)pSession;
    m_tUsingSessions.Detach(pListNode);
    pSession->OnFinal();
    m_tSessionPool.ReleaseMem(pListNode);
}

    //以下是Service专有的.
TInt32 CGatewayService::OnInit()
{
    //根据ServiceID来获取配置
    CDoid *pDoid = GetMyDoid();
    //肯定有
    
    CGatewayBasicConfig tConfig;
    TInt32 nRet = tConfig.ReadFile(pDoid->m_virtualIp,pDoid->m_srvId);
    if (nRet < SUCCESS)
    {
        printf_s("Read config file failed!");
        return nRet;
    }
    //然后生成Net
    m_pNet = CreateNet(m_pTaskMgr,&m_tParserFactory,NULL,tConfig.m_uMaxIncomingConnection4Listner,
                        (tConfig.m_uOutPutCacheInKBs*1024),(tConfig.m_uInputCacheInKBs*1024));
    //然后生成日志
    nRet = m_pLoggerManager->AddLogger(tConfig.m_szLoggerName,-1,tConfig.m_uWriteLoggerMask,tConfig.m_uPrint2ScreenLoggerMask);
    if (nRet < SUCCESS)
    {
        printf("Create Logger Failed!");
        DestoryNet(m_pNet);
        return OUT_OF_MEM;
    }
    m_pLogger = m_pLoggerManager->GetLogger(nRet);
    m_pIfComm = m_pOrb->GetCommunicator();
    m_pOrb->RegisterRun(m_pSkeleton,500);
    return SUCCESS;
}
    //结束是回调.
TInt32 CGatewayService::OnFinal()
{
    DestoryNet(m_pNet);
    m_pNet = NULL;
    m_pLoggerManager->ReleaseLogger(m_pLogger);
    m_pLogger = NULL;
    return SUCCESS;
}

    //定时  器到时了
TInt32 CGatewayService::OnTimer(TInt32 nTimerIdx,void *pData,TInt32 nTimeGap,TUInt32 nTimeNow)
{
    return SUCCESS;
}

    //以下是给Service使用的
    //定期的回调，可以注册循环时间，但只能有一个
TInt32 CGatewayService::OnRoutine(TUInt32 nRunCnt)
{
    TUInt32 nGap = GetClock()->GetTimeGap(m_uLastRoutineTime);
    if (nGap > 1000) //没1秒重新刷20个
    {
        m_uLastRoutineTime = GetClock()->GetLocalTime();
        TUInt32 uSize = m_tUsingSessions.size();
        if (uSize)
        {
            uSize >>= 7;
            ++uSize;
            while(uSize)
            {
                --uSize;
                CListNode<CGatewaySession> *pSession = m_tUsingSessions.pop_front();
                m_tUsingSessions.push_back(pSession);
                if (pSession)
                {
                    pSession->HeartBeat();
                }
                else
                {
                    break;
                }
            }
        }
    }
    return SUCCESS;
}
    //网络时间
TInt32 CGatewayService::OnNetEvent(CConnectionEvent *pEvent)
{
    return SUCCESS;
}

TInt32 CGatewayService::InitService(IfOrb* pOrb,IfTaskMgr *pIfTaskMgr,IfLoggerManager *pIfLoggerMgr)
{
    m_pTaskMgr = pIfTaskMgr;
    m_pLoggerManager = pIfLoggerMgr;
    m_pOrb = pOrb;
    m_pClock = pOrb->GetClock();
    m_uLastRoutineTime = GetClock()->GetLocalTime();
    return SUCCESS;
}

IfConnectionCallBack *CGatewayService::OnNewConnection(CConPair *pPair)
{
    if (m_tUsingSessions.size() < m_nMaxConnections)
    {
        CListNode<CGatewaySession> *pMem = m_tSessionPool.GetMem();
        if (pMem)
        {
            m_tUsingSessions.push_front(pMem);
            return pMem;
        }
    }
    return NULL;
}

CDoid *CGatewayService::FindService(TUInt32 uServiceId)
{
    return m_tServiceRoute.FindService(uServiceId);
}

TInt32 CGatewayService::AddRoute(CDoid *pDoid,TUInt32 uSrvId,TUInt32 uBegin,TUInt32 uEnd,TUInt32 uPriority)
{
    TInt32 nRet = m_tServiceRoute.AddRoute(pDoid,uSrvId,uBegin,uEnd,uPriority);
    if (nRet < SUCCESS)
    {
        CDoid *pRegister = GetCallerDoid();
        char szBufferRegister[64];
        pRegister->ToStr(szBufferRegister);
        char szBufferDoid[64];
        pDoid->ToStr(szBufferDoid);
        if (nRet < en_gw_error_begin)
        {
            LOG_RUN((-nRet),"Register:%s ,doid:%s, uSrvId:%u,uBegin:%u,uEnd:%u,uPriority:%u",szBufferRegister,szBufferDoid,uSrvId,uBegin,uEnd,uPriority);
        }
        else
        {
            LOG_CRITICAL((-nRet),"Register:%s ,doid:%s, uSrvId:%u,uBegin:%u,uEnd:%u,uPriority:%u",szBufferRegister,szBufferDoid,uSrvId,uBegin,uEnd,uPriority);
        }
    }
    return nRet;
}

CService *InitService(IfOrb* pStubCenter,IfTaskMgr *pIfTaskMgr,IfLoggerManager *pIfLoggerMgr)
{
    CGatewayService *p = new CGatewayService();
    TInt32 nRet = p->InitService(pStubCenter,pIfTaskMgr,pIfLoggerMgr);
    if (nRet < SUCCESS)
    {
        delete p;
        p = NULL;
    }
    return p;
}

TInt32 ReleaseService(CService* pService)
{
    CGatewayService *pGatewayService = dynamic_cast<CGatewayService *>(pService);
    pGatewayService->OnFinal();
    if (pService)
    {
        return -1;
    }
    delete pGatewayService;
    return SUCCESS;
}


//查找默认的服务入口点，即这些服务不需要鉴权和注册也能拥有，比如登陆



}
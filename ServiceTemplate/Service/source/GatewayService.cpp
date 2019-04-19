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
//ע����� 
TInt32 CGatewayService::RegisterService(TUInt32 uServiceId,TUInt32 uServicBegin,TUInt32 uEnd,TUInt32 uPriority,OctSeq<TUInt16>& tServiceName)
{
    AddRoute(GetCallerDoid(),uServiceId,uServicBegin,uEnd,uPriority);
    
    return SUCCESS;//��ʾ������.
}
//ע������
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
//���͹㲥������Ϣ
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

//��ʼ���յ�½��������ʹ�á����ط������������ڲ�������Э��������ɺ���gateway��ʼ���յ�½.
TInt32 CGatewayService::StartLogin(TUInt32 uIp,TUInt16 nListeningPort,TUInt16 nMaxConnection)
{
    if ((0 == m_uListeningPort)&&(0 == m_uIp))
    {
        IfListenerCallBack *pCallBack = this; //����תһ�£���λ���ԣ�Listen�Ĳ���void*,����IfListenerCallBack*
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
        //�ظ�����
        CDoid *pRegister = GetCallerDoid();
        char szBufferRegister[64];
        pRegister->ToStr(szBufferRegister);
        LOG_RUN(en_restart_listening,"Restart listening from doid:%s , New Ip:%u,port:%u,max connection:%u",szBufferRegister,uIp,(TUInt32)nListeningPort,(TUInt32)nMaxConnection);
    }
    return SUCCESS;
}
//ֹͣ�����½��������ʹ�á���ʼͣ��.���ʱ��Gatewayֻ��ֹͣ�����µ����ӣ������ӻ���ά�ֵ�
TInt32 CGatewayService::StopLogin()
{
    char szBuffer[64];
    GetCallerDoid()->ToStr(szBuffer);
    if (m_pListener)
    {
        LOG_RUN(en_stop_listening,"Caller(%s) stop Listening",szBuffer);
        m_pNet->StopListening(m_pListener); //�϶��ɹ�.
        m_pListener = NULL;
    }
    else
    {
        LOG_RUN(en_listening_not_started,"Caller(%s) try stop Listening,but there's no listening.",szBuffer);
    }
    return SUCCESS;
}
//�ر����пͻ�������
TInt32 CGatewayService::DisconnectedAllClient()
{
    return SUCCESS;
}

void CGatewayService::OnDisconnected(CGatewaySession *pSession,IfParser *pParser,IfCryptor *pCryptor,TInt32 uReason)
{
    //pParser \ pCryptor�������ͷ�.
    LOG_RUN(en_on_disconnected,"OnDisconnected,UserId:%llu,SystemId:%u,Reason%d",pSession->GetUserId(),pSession->GetSystemId(),uReason);

    m_pOrb->UnRegisterObj(pSession->GetSkeleton());
    CListNode<CGatewaySession> *pListNode = (CListNode<CGatewaySession> *)pSession;
    m_tUsingSessions.Detach(pListNode);
    pSession->OnFinal();
    m_tSessionPool.ReleaseMem(pListNode);
}

    //������Serviceר�е�.
TInt32 CGatewayService::OnInit()
{
    //����ServiceID����ȡ����
    CDoid *pDoid = GetMyDoid();
    //�϶���
    
    CGatewayBasicConfig tConfig;
    TInt32 nRet = tConfig.ReadFile(pDoid->m_virtualIp,pDoid->m_srvId);
    if (nRet < SUCCESS)
    {
        printf_s("Read config file failed!");
        return nRet;
    }
    //Ȼ������Net
    m_pNet = CreateNet(m_pTaskMgr,&m_tParserFactory,NULL,tConfig.m_uMaxIncomingConnection4Listner,
                        (tConfig.m_uOutPutCacheInKBs*1024),(tConfig.m_uInputCacheInKBs*1024));
    //Ȼ��������־
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
    //�����ǻص�.
TInt32 CGatewayService::OnFinal()
{
    DestoryNet(m_pNet);
    m_pNet = NULL;
    m_pLoggerManager->ReleaseLogger(m_pLogger);
    m_pLogger = NULL;
    return SUCCESS;
}

    //��ʱ  ����ʱ��
TInt32 CGatewayService::OnTimer(TInt32 nTimerIdx,void *pData,TInt32 nTimeGap,TUInt32 nTimeNow)
{
    return SUCCESS;
}

    //�����Ǹ�Serviceʹ�õ�
    //���ڵĻص�������ע��ѭ��ʱ�䣬��ֻ����һ��
TInt32 CGatewayService::OnRoutine(TUInt32 nRunCnt)
{
    TUInt32 nGap = GetClock()->GetTimeGap(m_uLastRoutineTime);
    if (nGap > 1000) //û1������ˢ20��
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
    //����ʱ��
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


//����Ĭ�ϵķ�����ڵ㣬����Щ������Ҫ��Ȩ��ע��Ҳ��ӵ�У������½



}
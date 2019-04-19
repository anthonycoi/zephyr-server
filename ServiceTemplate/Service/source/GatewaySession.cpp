#include "../include/GatewaySession.h"
#include "../../Interface/include/IfConnectingSkeleton.h"
#include "../include/GatewayService.h"
#include <time.h>

namespace Zephyr
{

IMPLEMENT_START_HANDLE_INTERFACE(CGatewaySession)
IMPLEMENT_HANDLE_INTERFACE(IfConnecting)
IMPLEMENT_END_HANDLE_INTERFACE(CGatewaySession)


CGatewaySession::CGatewaySession()
{
    OnInit();
}

void CGatewaySession::Init(CGatewayService *pService)
{
    m_pService = pService;
    m_tServiceRoute.Init(pService->GetRoutePool());
    m_uLastOprTime = pService->GetClock()->GetLocalTime();
}

void CGatewaySession::OnConnected(TUInt32 uIp,TUInt16 uPortId)
{
    m_uIp = uIp;
    m_uPort = uPortId;
    m_enState = en_shake_hands;
}

TInt32 CGatewaySession::OnRecv(TUChar *pMsg, TUInt32 msgLen)
{
    CMessageHeader::UnMsgInfo *pMsgInfo = (CMessageHeader::UnMsgInfo *)pMsg;
    TUInt32 uMsgId = pMsgInfo->m_methodId;
    if (pMsgInfo->m_msgBodyLength + sizeof(CMessageHeader::UnMsgInfo) == msgLen) //����ǿ϶���
    {
        CDoid *pDoid = m_tServiceRoute.FindService(uMsgId);
        if (pDoid)
        {
            CMessageHeader *pMsgHeader = PrepareMsg(pMsgInfo->m_msgBodyLength,uMsgId,pDoid,1,false);
            if (pMsg)
            {
                memcpy(pMsgHeader->GetBody(),(pMsg+sizeof(CMessageHeader::UnMsgInfo)),pMsgInfo->m_msgBodyLength);
                return SendMsg(pMsgHeader);
            }
        }
    }
    return msgLen;
}
    //virtual TInt32 OnRecvIn2Piece(TUChar *pMsg, TUInt32 msgLen,TUChar *pMsg2,TUInt32 msgLen2) = 0;
    //�������Զ���factory����parser��crypter,��Ӧ�ò��������������������
    //Ӧ�ò�Ӧ����ȷ֪��IfParser �� IfCryptor��ʵ���࣬����OnConnected��ʱ������������.
    //�������ֵen_if_connection_callback_on_connected_release_parser �����ˣ���ôparser�ᱻ�ͷ�
    //�������ֵen_if_connection_callback_on_connected_release_cryptor �����ˣ���ôpCryptor�ᱻ�ͷ�
    //IfConnection *pIfConnection������ʵ�ʽ�����ʱ���ٴ���Ӧ�ò㡣
TInt32 CGatewaySession::OnConnected(IfConnection *pIfConnection,IfParser *pParser,IfCryptor *pCryptor)
{
    m_pIfConnection = pIfConnection;
    m_pParser = pParser;
    m_pCryptor = pCryptor;
    return SUCCESS;
}
    //�κ�socket�쳣�����Զ��ر���������
TInt32 CGatewaySession::OnDissconneted(TInt32 erroCode)
{
    m_pIfConnection = NULL;
    //ע�⣬�������֮�󣬿����ڴ漴�ͷ��ˣ�����������������.
    m_pService->OnDisconnected(this,m_pParser,m_pCryptor,erroCode);
    return SUCCESS;
}

    //�ڳ�ʼ����ʱ��ᱻ��.
TInt32 CGatewaySession::OnInit()
{
    m_uUserId = 0;
    m_uSystemId = 0;
    m_uLastOprTime = time(NULL);
    m_enState = en_connection_not_using;
    m_uIp = 0;
    m_uPort = 0;
    m_pIfConnection = NULL;
    m_pParser = NULL;
    m_pCryptor = NULL;
    return SUCCESS;
}
    //�����ǻص�.
TInt32 CGatewaySession::OnFinal()
{
    m_tServiceRoute.OnFinal();
    m_enState = en_connection_not_using;
    m_uIp = 0;
    m_uPort = 0;
    m_pIfConnection = NULL;
    m_pParser = NULL;
    m_pCryptor = NULL;
    return SUCCESS;
}

TInt32 CGatewaySession::RegisterService(CDoid *pDoid,TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd,TUInt32 uPriority)
{
    TInt32 nRet = m_tServiceRoute.AddRoute(pDoid,uServiceId,uServiceIdBegin,uServcieIdEnd,uPriority);
    if (nRet < SUCCESS)
    {
        CDoid *pRegister = GetCallerDoid();
        char szBufferRegister[64];
        pRegister->ToStr(szBufferRegister);
        char szBufferDoid[64];
        pDoid->ToStr(szBufferDoid);
        if (nRet < en_gw_error_begin)
        {
            LOG_RUN((-nRet),"Register:%s ,doid:%s, uSrvId:%u,uBegin:%u,uEnd:%u,uPriority:%u",szBufferRegister,szBufferDoid,uServiceId,uServiceIdBegin,uServcieIdEnd,uPriority);
        }
        else
        {
            LOG_CRITICAL((-nRet),"Register:%s ,doid:%s, uSrvId:%u,uBegin:%u,uEnd:%u,uPriority:%u",szBufferRegister,szBufferDoid,uServiceId,uServiceIdBegin,uServcieIdEnd,uPriority);
        }
    }
    return nRet;
}

TInt32 CGatewaySession::UnregisterService(TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd)
{
    CDoid *pRegister = GetCallerDoid();
    TInt32 nRet = m_tServiceRoute.RmvRoute(pRegister,uServiceId,uServiceIdBegin,uServcieIdEnd);
    if (nRet < SUCCESS)
    {
        char szBufferRegister[64];
        pRegister->ToStr(szBufferRegister);
        if (nRet < en_gw_error_begin)
        {
            LOG_RUN((-nRet),"UnRegister from:%s , uSrvId:%u,uBegin:%u,uEnd:%u,",szBufferRegister,uServiceId,uServiceIdBegin,uServcieIdEnd);
        }
        else
        {
            LOG_CRITICAL((-nRet),"UnRegister from :%s , uSrvId:%u,uBegin:%u,uEnd:%u,",szBufferRegister,uServiceId,uServiceIdBegin,uServcieIdEnd);
        }
    }
    return SUCCESS;
}
    //ע��㲥
TInt32 CGatewaySession::RegisterTeam(TUInt32 uTeamID)
{
    return SUCCESS;
}

TInt32 CGatewaySession::Disconnect(TUInt32 uReason)
{
    m_pIfConnection->Disconnect();
    OnDissconneted(uReason);
    return SUCCESS;
}
TInt32 CGatewaySession::SetId(TUInt32 uId)
{
    m_uSystemId = uId;
    return SUCCESS;
}

TInt32 CGatewaySession::CheckId(TUInt32 uId)
{
    if (m_uSystemId != uId)
    {
        LOG_RUN(en_system_id_unmatch,"System Id incorrect! local system Id :%u, test id:%u",m_uSystemId,uId);
        Disconnect((-en_system_id_unmatch));
    }
    return SUCCESS;
}


void CGatewaySession::HeartBeat()
{
    TUInt32 nGap = m_pService->GetClock()->GetTimeGap(m_uLastOprTime);
    if (nGap > 30000) //����ʱ��30��
    {
        SendHeartBeat();
        m_uLastOprTime = m_pService->GetClock()->GetLocalTime();
    }
}
void CGatewaySession::SendHeartBeat()
{

}

IfLogger *CGatewaySession::GetLogger()
{
#ifdef _DEBUG
    if (NULL == m_pService)
    {
        return NULL;
    }
#endif
    return m_pService->GetLogger();
}

}

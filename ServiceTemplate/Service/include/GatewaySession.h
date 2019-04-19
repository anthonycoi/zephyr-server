#ifndef __ZEPHYR_GATEWAY_SESSION_H__
#define __ZEPHYR_GATEWAY_SESSION_H__

#include "Public/include/TypeDef.h"
#include "Public/include/SysMacros.h"
#include "Public/Orb/include/Session.h"
#include "../../Interface/Interface/IfConnecting.h"
#include "Public/include/IdlHeaderFile.h"
#include "Public/tpl/include/TplList.h"
#include "Route.h"
#include "Public/Interface/App/include/IfNetApp.h"
#include "Public/tpl/include/TplMultiMap.h"
#include "GatewayLogger.h"
#include "RouteMap.h"
namespace Zephyr
{

class CGatewayService;

class CGatewaySession : public CSession
                      , public IfConnectionCallBack    //Net�Ļص�
                      , public IfConnecting    //Զ�̽ӿ�
{
public:
    enum EnSessionState
    {
        en_connection_not_using,   //δʹ��
        en_shake_hands,            //����
        en_connection_established, //���Ӵ���
        en_trying_to_disconnected, //���Զ���
    };
private:
    TUInt64         m_uUserId;
    CRouteMap       m_tServiceRoute;
    CGatewayService *m_pService;
    TUInt32          m_uLastOprTime;
    EnSessionState   m_enState;
    TUInt32         m_uIp;
    TUInt16         m_uPort;

    //���ID��ϵͳ�������ʱID������ͻ�����
    TUInt32         m_uSystemId;
    IfConnection    *m_pIfConnection;
    IfParser        *m_pParser;
    IfCryptor       *m_pCryptor;

public:
    TUInt64 GetUserId()
    {
        return m_uUserId;
    }
    TUInt32 GetSystemId()
    {
        return m_uSystemId;
    }
    DECALRE_HANDLE_INTERFCE

    CGatewaySession();
    //ֻ����һ��
    void Init(CGatewayService *pService);

    //����������ӽ���������ʱ��ص���.
    void OnConnected(TUInt32 uIp,TUInt16 uPortId);
public:
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
    void HeartBeat();
public:

    //�ڳ�ʼ����ʱ��ᱻ��.
    virtual TInt32      OnInit();
    //�����ǻص�.
    virtual TInt32      OnFinal();

    virtual TInt32 RegisterService(CDoid *pDoid,TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd,TUInt32 uPriority);
    virtual TInt32 UnregisterService(TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd);
    //ע��㲥
    virtual TInt32 RegisterTeam(TUInt32 uTeamID);
    virtual TInt32 Disconnect(TUInt32 uReason);
    virtual TInt32 SetId(TUInt32 uId);
    virtual TInt32 CheckId(TUInt32 uId);
    //ʹ��inline
    IfLogger *GetLogger();
private:
    void   SendHeartBeat();
    
    
};



}

#endif

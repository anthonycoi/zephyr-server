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
                      , public IfConnectionCallBack    //Net的回调
                      , public IfConnecting    //远程接口
{
public:
    enum EnSessionState
    {
        en_connection_not_using,   //未使用
        en_shake_hands,            //握手
        en_connection_established, //连接创建
        en_trying_to_disconnected, //尝试断链
    };
private:
    TUInt64         m_uUserId;
    CRouteMap       m_tServiceRoute;
    CGatewayService *m_pService;
    TUInt32          m_uLastOprTime;
    EnSessionState   m_enState;
    TUInt32         m_uIp;
    TUInt16         m_uPort;

    //这个ID是系统分配的零时ID，用完就会销毁
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
    //只调用一次
    void Init(CGatewayService *pService);

    //这个是在连接建立起来的时候回调的.
    void OnConnected(TUInt32 uIp,TUInt16 uPortId);
public:
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
    void HeartBeat();
public:

    //在初始化的时候会被调.
    virtual TInt32      OnInit();
    //结束是回调.
    virtual TInt32      OnFinal();

    virtual TInt32 RegisterService(CDoid *pDoid,TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd,TUInt32 uPriority);
    virtual TInt32 UnregisterService(TUInt32 uServiceId,TUInt32 uServiceIdBegin,TUInt32 uServcieIdEnd);
    //注册广播
    virtual TInt32 RegisterTeam(TUInt32 uTeamID);
    virtual TInt32 Disconnect(TUInt32 uReason);
    virtual TInt32 SetId(TUInt32 uId);
    virtual TInt32 CheckId(TUInt32 uId);
    //使用inline
    IfLogger *GetLogger();
private:
    void   SendHeartBeat();
    
    
};



}

#endif

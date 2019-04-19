#ifndef __ZEPHYR_COMM_COMM_MSG_PARSER_H__
#define __ZEPHYR_COMM_COMM_MSG_PARSER_H__
#include "TypeDef.h"
#include "..\Public\Interface\App\include\IfParser.h"
#include "..\Public\include\Message.h"

namespace Zephyr
{



class CMsgParser : public IfParser
{
protected:
    TUInt64 m_nrOfMsgRecived;
    TUInt32 m_connectedTime;
    TUInt32 m_ip;
public:
    CMsgParser()
    {
        OnInit();
    }
    TBOOL IsInUse()
    {
        if (m_ip)
        {
            return TRUE;
        }
        return FALSE;
    }
    void OnInit()
    {
        m_nrOfMsgRecived = 0;
        m_connectedTime  = 0;
    }
    TInt32 Init(TUInt32 ip)
    {
        OnInit();
        m_ip = ip;
        return SUCCESS;
    }
    void OnFinal()
    {
        m_nrOfMsgRecived = 0;
        m_connectedTime = 0;
        m_ip = 0;
    }
    virtual TInt32  GetMaxHeaderLength()
    {
        return sizeof(CMessageHeader);
    }
    virtual TInt32  GetMinHeaderLength()
    {
        return sizeof(CMessageHeader);
    }
    //如果GetMaxHeaderLength == GetMinHeaderLength，则headerLen就是GetMinHeaderLength，
    //注意GetMaxHeaderLength、GetMinHeaderLength只在连接初始化的时候调用一次，以后不会改变
    //否则得话，如果网络层积累的数据超过GetMinHeaderLength,则就回调这个函数，同时将实际收到的数据长度(<=GetMaxHeaderLength)写在headerLen中.
    //pNetData 只传给MaxHeaderLength的数据,应用层根据头信息，要返回给net此次需要上报的数据长度.
    //返回>0则表示成功，负数表示需要丢弃的字节数.
    //每次返回一条消息的长度，如果返回后，还有多余的数据，Net会多次调用
    virtual TInt32  OnRecv(TUChar *pNetData,TInt32 dataLen);
};
}

#endif

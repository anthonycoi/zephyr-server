#include "../include/GatewayParser.h"
#include "Public/include/TypeDef.h"
#include "Public/include/SysMacros.h"

#include "Public/include/Message.h"

namespace Zephyr
{
TInt32  CGatewayParser::GetMaxHeaderLength()
{
    return sizeof(CMessageHeader::UnMsgInfo);
}

TInt32  CGatewayParser::GetMinHeaderLength()
{
    return sizeof(CMessageHeader::UnMsgInfo);
}
    //如果GetMaxHeaderLength == GetMinHeaderLength，则headerLen就是GetMinHeaderLength，
    //注意GetMaxHeaderLength、GetMinHeaderLength只在连接初始化的时候调用一次，以后不会改变
    //否则得话，如果网络层积累的数据超过GetMinHeaderLength,则就回调这个函数，同时将实际收到的数据长度(<=GetMaxHeaderLength)写在headerLen中.
    //pNetData 只传给MaxHeaderLength的数据,应用层根据头信息，要返回给net此次需要上报的数据长度.
    //返回>0则表示成功，负数表示需要丢弃的字节数.
    //每次返回一条消息的长度，如果返回后，还有多余的数据，Net会多次调用
TInt32  CGatewayParser::OnRecv(TUChar *pNetData,TInt32 dataLen)
{
    CMessageHeader::UnMsgInfo msgInfo;
    memcpy(&msgInfo,pNetData,sizeof(CMessageHeader::UnMsgInfo));
    //msgInfo.m_data = *(TUInt64*)pNetData;
    TInt32 nLen = (msgInfo.m_msgBodyLength+sizeof(msgInfo));
    if (nLen>dataLen)
    {
        return nLen;
    }
    return 0;
}

}
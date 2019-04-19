/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 协议解析器接口
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/


#ifndef __ZEPHYR_IF_PARSER_H__
#define __ZEPHYR_IF_PARSER_H__

#include "../../../include/TypeDef.h"
//#include "Message.h"

namespace Zephyr
{

class IfParser
{
public:
    virtual TInt32  GetMaxHeaderLength() = 0;
    virtual TInt32  GetMinHeaderLength() = 0;
    //如果GetMaxHeaderLength == GetMinHeaderLength，则headerLen就是GetMinHeaderLength，
    //注意GetMaxHeaderLength、GetMinHeaderLength只在连接初始化的时候调用一次，以后不会改变
    //否则得话，如果网络层积累的数据超过GetMinHeaderLength,则就回调这个函数，同时将实际收到的数据长度(<=GetMaxHeaderLength)写在headerLen中.
    //pNetData 只传给MaxHeaderLength的数据,应用层根据头信息，要返回给net此次需要上报的数据长度.
    //返回>0则表示成功，负数表示需要丢弃的字节数.
    //每次返回一条消息的长度，如果返回后，还有多余的数据，Net会多次调用
    virtual TInt32  OnRecv(TUChar *pNetData,TInt32 dataLen) = 0;

    //virtual TInt32  OnSend(CMessageHeader *pMsg) = 0;
    //virtual TInt32  CodeMsg(CMessageHeader *pMsg,TUChar *pWBuff,TInt32 wBuffLength) = 0;
};


}

 #endif
 
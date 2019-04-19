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


#ifndef __ZEPHYR_IF_CRYPTOR_H__
#define __ZEPHYR_IF_CRYPTOR_H__

#include "../../../include/TypeDef.h"
//#include "Message.h"

namespace Zephyr
{

class IfCryptor
{
public:
    //加密
    //由网络层来调用。如果要修改请注意线程问题。
    virtual TInt32  Encrypt(TUChar *pNetData,TInt32 dataLength) = 0;
    //解密,这个时间是由net来的,在收到的时候解密,pNewData内存块请使用静态变量或栈变量,网络层并不删除
    virtual TInt32  Decrypt(TUChar *pNetData,TInt32 dataLength) = 0;
    //这个是释放
};

}

#endif

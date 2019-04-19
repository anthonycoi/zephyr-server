/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 协议解析器工厂接口
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __IF_CRYPTOR_FACTORY_H__
#define __IF_CRYPTOR_FACTORY_H__

#include "../../../include/TypeDef.h"
#include "IfCryptor.h"
#include "../../Platform/include/IfNetData.h"
namespace Zephyr
{

class IfCryptorFactory
{
public:
    virtual ~IfCryptorFactory() = 0;
    //根据CConPair来创建一个加密器，注意，这个方法是由应用层的线程来调用的，即在IfNet::Run中被回调
    virtual IfCryptor *GetCryptor(CConPair *pPair,TUInt32 connectionIdx) = 0;
    //释放IfCryptor
    virtual void ReleaseCryptor(IfCryptor *pIfCryptor) = 0;
};

}
 #endif
 
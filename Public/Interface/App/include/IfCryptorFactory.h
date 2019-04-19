/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: Э������������ӿ�
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
    //����CConPair������һ����������ע�⣬�����������Ӧ�ò���߳������õģ�����IfNet::Run�б��ص�
    virtual IfCryptor *GetCryptor(CConPair *pPair,TUInt32 connectionIdx) = 0;
    //�ͷ�IfCryptor
    virtual void ReleaseCryptor(IfCryptor *pIfCryptor) = 0;
};

}
 #endif
 
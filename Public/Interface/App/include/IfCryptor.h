/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: Э��������ӿ�
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
    //����
    //������������á����Ҫ�޸���ע���߳����⡣
    virtual TInt32  Encrypt(TUChar *pNetData,TInt32 dataLength) = 0;
    //����,���ʱ������net����,���յ���ʱ�����,pNewData�ڴ����ʹ�þ�̬������ջ����,����㲢��ɾ��
    virtual TInt32  Decrypt(TUChar *pNetData,TInt32 dataLength) = 0;
    //������ͷ�
};

}

#endif

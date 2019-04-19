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
    //���GetMaxHeaderLength == GetMinHeaderLength����headerLen����GetMinHeaderLength��
    //ע��GetMaxHeaderLength��GetMinHeaderLengthֻ�����ӳ�ʼ����ʱ�����һ�Σ��Ժ󲻻�ı�
    //����û�������������۵����ݳ���GetMinHeaderLength,��ͻص����������ͬʱ��ʵ���յ������ݳ���(<=GetMaxHeaderLength)д��headerLen��.
    //pNetData ֻ����MaxHeaderLength������,Ӧ�ò����ͷ��Ϣ��Ҫ���ظ�net�˴���Ҫ�ϱ������ݳ���.
    //����>0���ʾ�ɹ���������ʾ��Ҫ�������ֽ���.
    //ÿ�η���һ����Ϣ�ĳ��ȣ�������غ󣬻��ж�������ݣ�Net���ε���
    virtual TInt32  OnRecv(TUChar *pNetData,TInt32 dataLen) = 0;

    //virtual TInt32  OnSend(CMessageHeader *pMsg) = 0;
    //virtual TInt32  CodeMsg(CMessageHeader *pMsg,TUChar *pWBuff,TInt32 wBuffLength) = 0;
};


}

 #endif
 
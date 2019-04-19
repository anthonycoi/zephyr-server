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
    //���GetMaxHeaderLength == GetMinHeaderLength����headerLen����GetMinHeaderLength��
    //ע��GetMaxHeaderLength��GetMinHeaderLengthֻ�����ӳ�ʼ����ʱ�����һ�Σ��Ժ󲻻�ı�
    //����û�������������۵����ݳ���GetMinHeaderLength,��ͻص����������ͬʱ��ʵ���յ������ݳ���(<=GetMaxHeaderLength)д��headerLen��.
    //pNetData ֻ����MaxHeaderLength������,Ӧ�ò����ͷ��Ϣ��Ҫ���ظ�net�˴���Ҫ�ϱ������ݳ���.
    //����>0���ʾ�ɹ���������ʾ��Ҫ�������ֽ���.
    //ÿ�η���һ����Ϣ�ĳ��ȣ�������غ󣬻��ж�������ݣ�Net���ε���
    virtual TInt32  OnRecv(TUChar *pNetData,TInt32 dataLen);
};
}

#endif

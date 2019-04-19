#ifndef __ZEPHYR_GATEWAY_PARSER_H__
#define __ZEPHYR_GATEWAY_PARSER_H__

#include "Public/Interface/App/include/IfParser.h"

namespace Zephyr
{

//�ڿͻ��˷�����Ϣ��ʱ�����ǿ��԰�m_destDoid��m_srcDoid��������Ϊ����������������������Ĭ�ϵģ�����Ҫ�ٴ�

class CGatewayParser : public IfParser
{
public:
    virtual TInt32  GetMaxHeaderLength();
    virtual TInt32  GetMinHeaderLength();
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

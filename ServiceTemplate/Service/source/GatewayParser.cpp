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
    //���GetMaxHeaderLength == GetMinHeaderLength����headerLen����GetMinHeaderLength��
    //ע��GetMaxHeaderLength��GetMinHeaderLengthֻ�����ӳ�ʼ����ʱ�����һ�Σ��Ժ󲻻�ı�
    //����û�������������۵����ݳ���GetMinHeaderLength,��ͻص����������ͬʱ��ʵ���յ������ݳ���(<=GetMaxHeaderLength)д��headerLen��.
    //pNetData ֻ����MaxHeaderLength������,Ӧ�ò����ͷ��Ϣ��Ҫ���ظ�net�˴���Ҫ�ϱ������ݳ���.
    //����>0���ʾ�ɹ���������ʾ��Ҫ�������ֽ���.
    //ÿ�η���һ����Ϣ�ĳ��ȣ�������غ󣬻��ж�������ݣ�Net���ε���
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
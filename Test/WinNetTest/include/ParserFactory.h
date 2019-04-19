#ifndef __ZEPHYR_COMM_WIN_NET_TEST_PARSER_FACTORY_H__
#define __ZEPHYR_COMM_WIN_NET_TEST_PARSER_FACTORY_H__

#include "IfParser.h"
#include "IfParserFactory.h"


namespace Zephyr
{




class CTestparser : public IfParser
{
public:
    virtual TInt32  GetMaxHeaderLength()
    {
        return 4;
    }
    virtual TInt32  GetMinHeaderLength()
    {
        return 4;
    }
    //���GetMaxHeaderLength == GetMinHeaderLength����headerLen����GetMinHeaderLength��
    //ע��GetMaxHeaderLength��GetMinHeaderLengthֻ�����ӳ�ʼ����ʱ�����һ�Σ��Ժ󲻻�ı�
    //����û�������������۵����ݳ���GetMinHeaderLength,��ͻص����������ͬʱ��ʵ���յ������ݳ���(<=GetMaxHeaderLength)д��headerLen��.
    //pNetData ֻ����MaxHeaderLength������,Ӧ�ò����ͷ��Ϣ��Ҫ���ظ�net�˴���Ҫ�ϱ������ݳ���.
    //����>0���ʾ�ɹ���������ʾ��Ҫ�������ֽ���.
    //ÿ�η���һ����Ϣ�ĳ��ȣ�������غ󣬻��ж�������ݣ�Net���ε���
    virtual TInt32  OnRecv(TUChar *pNetData,TInt32 dataLen)
    {
        TInt32 len = *((TInt32*)pNetData);
        return len;
    }

};

class CTestParserFactory : public IfParserFactory
{
private:
    CTestparser m_parser;
public:
    CTestParserFactory()
    {
    
    }
    virtual IfParser *GetParser(CConPair *pPair,TUInt32 connectionIdx)
    {
        return &m_parser;
    }
    virtual void ReleaseParser(IfParser *pParser)
    {
        ;
    }
};

}


#endif

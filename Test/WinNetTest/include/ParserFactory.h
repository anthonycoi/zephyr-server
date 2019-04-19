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
    //如果GetMaxHeaderLength == GetMinHeaderLength，则headerLen就是GetMinHeaderLength，
    //注意GetMaxHeaderLength、GetMinHeaderLength只在连接初始化的时候调用一次，以后不会改变
    //否则得话，如果网络层积累的数据超过GetMinHeaderLength,则就回调这个函数，同时将实际收到的数据长度(<=GetMaxHeaderLength)写在headerLen中.
    //pNetData 只传给MaxHeaderLength的数据,应用层根据头信息，要返回给net此次需要上报的数据长度.
    //返回>0则表示成功，负数表示需要丢弃的字节数.
    //每次返回一条消息的长度，如果返回后，还有多余的数据，Net会多次调用
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

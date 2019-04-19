#ifndef __ZEPHYR_GATEWAY_PARSER_FACTORY_H__
#define __ZEPHYR_GATEWAY_PARSER_FACTORY_H__

#include"Public/Interface/App/include/IfParserFactory.h"

#include "Public/include/TypeDef.h"
#include "./GatewayParser.h"


namespace Zephyr
{
class CGatewayParserFactory : public IfParserFactory
{
private:
    CGatewayParser m_tCommonParser;
public:
    //根据CConPair来创建一个解包器，注意，这个方法是由应用层的线程来调用的，即在IfNet::Run中被回调;
    virtual IfParser *GetParser(CConPair *pPair,TUInt32 connectionIdx);
    virtual void ReleaseParser(IfParser *pParser);
};

}


#endif

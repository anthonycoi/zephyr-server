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
    //����CConPair������һ���������ע�⣬�����������Ӧ�ò���߳������õģ�����IfNet::Run�б��ص�;
    virtual IfParser *GetParser(CConPair *pPair,TUInt32 connectionIdx);
    virtual void ReleaseParser(IfParser *pParser);
};

}


#endif

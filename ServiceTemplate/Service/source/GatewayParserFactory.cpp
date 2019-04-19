#include "../include/GatewayParserFactory.h"

namespace Zephyr
{

IfParser *CGatewayParserFactory::GetParser(CConPair *pPair,TUInt32 connectionIdx)
{
    return &m_tCommonParser;
}

void CGatewayParserFactory::ReleaseParser(IfParser *pParser)
{
    //¶«Å·thing

}

}
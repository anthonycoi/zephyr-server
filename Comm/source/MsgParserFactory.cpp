#include "include\MsgParserFactory.h"

#ifdef _DEBUG
#include "stdio.h"
#endif

namespace Zephyr
{
CMsgParserFactory::CMsgParserFactory(TUInt32 nrOfConnection)
{
    NEW(m_pParsers,CMsgParser,nrOfConnection);
    m_nNrOfParser = nrOfConnection;
}

IfParser *CMsgParserFactory::GetParser(CConPair *pPair,TUInt32 connectionIdx)
{
    if (connectionIdx > m_nNrOfParser)
    {
#ifdef _DEBUG
    printf("Connection Id %d is out of range!",connectionIdx);
#endif
        return NULL;
    }
    CMsgParser *pResult = (m_pParsers + connectionIdx);
    pResult->Init(pPair->GetRemoteIp());
    return pResult;
}

void CMsgParserFactory::ReleaseParser(IfParser *pIfParser)
{
    CMsgParser *pRel = (CMsgParser*)pIfParser;
    pRel->OnFinal();
}

}

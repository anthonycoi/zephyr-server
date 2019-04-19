#ifndef __ZEPHYR_COMM_COMM_MSG_PARSER_FACTORY_H__
#define __ZEPHYR_COMM_COMM_MSG_PARSER_FACTORY_H__
#include "..\Public\include\TypeDef.h"
#include "..\Public\Interface\App\include\IfParserFactory.h"
#include "..\Public\Interface\App\include\IfParser.h"
#include "MsgParser.h"
namespace Zephyr
{
class CMsgParserFactory : public IfParserFactory
{
protected:
    CMsgParser *m_pParsers;
    TUInt32     m_nNrOfParser;
public:
    CMsgParserFactory(TUInt32 nrOfConnection);
public:
    virtual IfParser *GetParser(CConPair *pPair,TUInt32 connectionIdx);
    virtual void ReleaseParser(IfParser *pParser);
};
}
#endif

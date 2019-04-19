/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: Э������������ӿ�
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __IF_PARSER_FACTORY_H__
#define __IF_PARSER_FACTORY_H__

#include "../../../include/TypeDef.h"
#include "IfParser.h"
#include "../../Platform/include/IfNetData.h"
namespace Zephyr
{

class IfParserFactory
{
public:
    //virtual ~IfParserFactory() = 0;
    //����CConPair������һ���������ע�⣬�����������Ӧ�ò���߳������õģ�����IfNet::Run�б��ص�;
    virtual IfParser *GetParser(CConPair *pPair,TUInt32 connectionIdx) = 0;
    virtual void ReleaseParser(IfParser *pParser) = 0;
};

}
 #endif
 
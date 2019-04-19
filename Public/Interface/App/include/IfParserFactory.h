/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfTask.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 协议解析器工厂接口
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
    //根据CConPair来创建一个解包器，注意，这个方法是由应用层的线程来调用的，即在IfNet::Run中被回调;
    virtual IfParser *GetParser(CConPair *pPair,TUInt32 connectionIdx) = 0;
    virtual void ReleaseParser(IfParser *pParser) = 0;
};

}
 #endif
 
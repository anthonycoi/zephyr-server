/*-------------------------------------------------------------
 Copyright (C)| 
 File: method.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ����ʹ�õ�namespace,namespace ����Ψһ�Ĳ�ͬ�ǣ�����дpublic\protected\private�ؼ���
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/


#ifndef __ZEPHYR_IDL_GENERATOR_NAME_SPACE_H__
#define __ZEPHYR_IDL_GENERATOR_NAME_SPACE_H__
#include "./baseElement.h"

namespace Zephyr
{


class CNamespace : public CBaseElement
{
public:
    DECLARE_STATIC_CLASS_POOL(CNamespace);
public:
    CNamespace()
    {
        m_nElmentType = raw_namespace_type;
    }
    virtual TInt32 Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles);

    virtual TInt32 GenerateSkeleton(const char *pPath);
    virtual TInt32 GenerateStub(const char *pPaht);

    //�϶��� Type + [&|*] + name +['('+[Type+[*|&]+[NAME+[=+(Num|Str|Char)]]]+[,+TYPE+[&|*]+[NAME]]+')'] + ';'
    //Type =  [const|volatile|static]+[(NAMESPACE|CLASSNAME)+::]+name
    int findAType(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles);//Type;
    int findAComma(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles); //,
    int findAName(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles); //name;
    int findTypeOpr(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles);//[*|**|*&|&|**];
    int findMethod(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles); //[(+[Type+[*|&]+[NAME+[=[Num|Str|Char]]]]+[,+TYPE+[&|*]+[NAME]])];
    int findADefaultValue(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles); //[=[Num|Str|Char]];
    
    //��ʵnLength��ʵûɶ��
    TInt32 GenerateNamespaceCode(char *pBuff,int nLength);
    TInt32 GenerateNamespaceCodeEnd(char *pBuff,int nLength);
};

}

#endif

/*-------------------------------------------------------------
 Copyright (C)| 
 File: HeaderFile.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 一个文件
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_SERVICE_CONTANER_IDL_GENERATOR_HEADER_FILE_H__
#define __ZEPHYR_SERVICE_CONTANER_IDL_GENERATOR_HEADER_FILE_H__
#include "../../Public/include/TypeDef.h"
#include "./baseElement.h"
#include "../../Public/tpl/include/tplmap.h"

namespace Zephyr
{


class CHeaderFile : public CBaseElement
{
public:
    DECLARE_STATIC_CLASS_POOL(CHeaderFile);
protected:
    CBaseElement **m_ppElements;
    char         **m_ppWords;
    EnType         *m_pWordsTypes;
    char          *m_pszFileInPiece;
    TUInt32       m_nNrOfWords;
    TUInt32       m_nNrOfElements;
    TUInt32       m_nFileLength;
    char    *m_pszFile;
    std::string   m_szFileName;
    
public:
    static string sm_szServiceName;
    static const char *m_pFileName;
    CHeaderFile();
    ~CHeaderFile();
    TInt32 GeneratorIdl(const char *pFileName,const char *pPath);
    
private:
    //读取文件 
    TInt32 ReadFile(const char *pFileName);

    //分词
    TInt32 DividIntoWords();

    
    TInt32 GeneratorIdlCode();

    TBOOL  IsDivider(char c);
    TBOOL  IsEnter(char c);
    TBOOL  IsOperator(char c);
    TBOOL  IsAlphabet(char c);
    TBOOL  IsNum(char c);
    TBOOL  IsBiggerMark(char c)
    {
        if ('>' == c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsSmallerMark(char c)
    {
        if ('<' == c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL  IsSlashMark(char c)
    {
        if ('/'==c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsBackSlash(char c)
    {
        if ('\\'==c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL  IsStarMark(char c)
    {
        if ('*'==c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsSingleQuote(char c)
    {
        if ('\''==c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsQuote(char c)
    {
        if ('"'==c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsBlanket1(char c)
    {
        if('('==c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsBlanket2(char c)
    {
        if (')'==c)
        {
            return TRUE;
        }
        return FALSE;
    }
    TBOOL IsSemicolon(char c)
    {
         if (';' == c)
         {
             return TRUE;
         }
         return FALSE;
    }
    virtual TInt32 Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles);
    virtual const char *GetHierachyName()
    {   //什么都不需要
        return "";
    }
    virtual TInt32 GenerateSkeleton(const char *pPath);
    virtual TInt32 GenerateStub(const char *pPaht);

    void RemoveAllNumLine(); //删除所有'#'
    void RemoveAllCommentsAndMakeConstStr();
    void RemoveAllType(EnType enType);
    void RemoveAllTypeDef();
};

}

#endif

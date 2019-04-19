/*-------------------------------------------------------------
 Copyright (C)| 
 File: method.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 编译使用的方法的类
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_SERVICE_CONTANER_IDL_GENERATOR_METHOD_ELEMENT_H__
#define __ZEPHYR_SERVICE_CONTANER_IDL_GENERATOR_METHOD_ELEMENT_H__

#include "baseElement.h"
#include "../../Public/tpl/include/TplPool.h"
#include "./FullTypeClass.h"

namespace Zephyr
{

class CMethod : public CBaseElement
{
public:
    DECLARE_STATIC_CLASS_POOL(CMethod);
protected:
    int              m_nIsVirtual;  //0 不是，1是虚的，2是纯虚
    //name
    CFullTypeDef    *m_pFullRetType;;
    string          m_szFullName;
public:
    CFullTypeDef *GetFullRetType()
    {
        return m_pFullRetType;
    }
    void InitFullName(const char* psz)
    {
        m_szFullName = psz;
    }
    CMethod()
    {
        m_nIsVirtual = 0;
        m_pFullRetType = NULL; 
        m_nElmentType = raw_method_type;
    }
//     CMethodElement *GetMethod()
//     {
//         return m_pBelongs2;
//     }
    void AddVirtualDeg()
    {
        ++m_nIsVirtual;
    }

    void SetRtnType(CFullTypeDef *p)
    {
        m_pFullRetType = p;
    }
    virtual TInt32 Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles);
    virtual const char *GetHierachyName();
    TInt32 GetFullMethodTxt(char *pszBuff,int nLength);
    
    TInt32 GetFullSkeletonMethodTxt(char *pszBuff,int nLength);
    TInt32 GetMethodFunPtTxt(char *pszBuff,int nLength);
    TInt32 GenerateStubSourceCode(char *pszBuff,int nLength);
    TInt32 GenerateSkeletonSourceCode(char *pszBuff,int nLength);
//     TInt32 GeneratePresenterHeaderFile(char *pPath); //这个其实没必要的，
//     TInt32 GeneratePresenterSourceFile(char *pPath);

    TInt32 GetMethodIdStr(char *pBuff,int nlength);

};

}

#endif


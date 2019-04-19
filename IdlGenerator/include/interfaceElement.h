/*-------------------------------------------------------------
 Copyright (C)| 
 File: interfaceElement.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 编译使用的接口类
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_SERVICE_CONTANER_IDL_GENERATOR_INTERFACE_ELEMENT_H__
#define __ZEPHYR_SERVICE_CONTANER_IDL_GENERATOR_INTERFACE_ELEMENT_H__

#include "baseElement.h"


namespace Zephyr
{


class CHeaderFile;

class CInterfaceElement : public CBaseElement
{
public:
    DECLARE_STATIC_CLASS_POOL(CInterfaceElement);
private:
    CHeaderFile *m_pOwner;

    enum EnState
    {
        en_class_key_word,    //class
        en_class_name,        //CName
        en_class_name_risk,   //:
        en_class_derive_virtual, //virtual 
        en_class_derive_class_name, //CDerive 
        en_class_right_brace,     //
        en_class_left_brace,
    };
public:
    CInterfaceElement();
    //class Name {  public : method1 method2 ...} ;
    virtual TInt32 Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles);
    virtual const char *GetHierachyName(void);

    virtual TInt32 GenerateSkeleton(const char *pPath);
    virtual TInt32 GenerateStub(const char *pPath);
    virtual TInt32 GenerateMethodId(const char *pPath);

    //void OnError(int nProcess2);
    EnState GetState(char *pAlphabets);
    int HandleAStatement(char **ppElements,EnType *pTypes,int& nProcess2,int nTotalEles);
    TInt32 GetMethodIdStr(char *pBuff,int nLength);
private:
    TInt32 GenerateMethodIdFile(const char *pPath,int i);
    TInt32 GenerateStubHeaderFile(const char *pPath);
    TInt32 GenerateStubSourceFile(const char *pPath);
    TInt32 GenerateSkeletonHeaderFile(const char *pPath);
    TInt32 GenerateSkeletonSourceFile(const char *pPath);
};

}

#endif

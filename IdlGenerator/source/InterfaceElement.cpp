#include "../include/HeaderFile.h"
#include "../include/interfaceElement.h"
#include "../include/FullTypeClass.h"
#include "../include/IdlGeneratorErrorCode.h"
#include "../include/Method.h"
#include "../include/Parameter.h"
#include "../include/nameSpace.h"

#include <vector>


namespace Zephyr
{

IMPLEMENT_STATIC_CLASS_POOL(CInterfaceElement)


CInterfaceElement::CInterfaceElement()
{
    m_nElmentType = raw_interface_type;
}


CInterfaceElement::EnState CInterfaceElement::GetState(char *pAlphabets)
{
    return en_class_key_word;
}

TInt32 CInterfaceElement::Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles)
{
    EnState enLastState = en_class_key_word;
    EnType enterAndDividerTypes[2] = {enter_type,divider_type};
    struct Tt
    {
        char *m_pszTxt;
        EnState   m_enType;
    };
    vector<char*> pHeap;
    int nOld = nProcess2;
    
    while (nProcess2 < nTotalEles)
    {
        //忽律所有
        switch (enLastState)
        {
        case en_class_key_word:
            {
                if (pTypes[nProcess2] == alphabet_type)
                {
//                     m_szName.clear();
//                     if (m_pFather)
//                     {
//                         const char *pFathersName = m_pFather->GetName();
//                         if (pFathersName)
//                         {
//                             m_szName = pFathersName;
//                         }
//                     }
                    m_szName = ppElements[nProcess2];
                    enLastState = en_class_name;
                    ++nProcess2;
                }
                else
                {
                    char *pAt =NULL;
                    if (nProcess2 < nTotalEles)
                    {
                        pAt = ppElements[nTotalEles];
                    }
                    OnError(pAt);
                    return -1;
                }
            }
            break; //case en_class_key_word:
        case         en_class_name:        //CName
            {
                
                if (pTypes[nProcess2] == operator_type)
                {
                    if (ppElements[nProcess2][0] == ':')
                    {
                        ++nProcess2;
                        enLastState = en_class_name_risk;
                    }
                    else
                    {
                        if (pTypes[nProcess2] == operator_type)
                        {
                            if (ppElements[nProcess2][0] == ';') //只是个声明
                            {
                                AddType(this);
                                ++nProcess2;
                                return nProcess2;
                            }
                            else if (ppElements[nProcess2][0] == '{')
                            {
                                enLastState = en_class_right_brace;
                                ++nProcess2;
                            }
                            else
                            {
                                char *pAt =NULL;
                                if (nProcess2 < nTotalEles)
                                {
                                    pAt = ppElements[nTotalEles];
                                }
                                OnError(pAt);
                                return -1;
                            }
                        }
                        else
                        {
                            char *pAt =NULL;
                            if (nProcess2 < nTotalEles)
                            {
                                pAt = ppElements[nTotalEles];
                            }
                            OnError(pAt);
                            return -1;
                        }
                    }
                }
                else
                {
                    char *pAt =NULL;
                    if (nProcess2 < nTotalEles)
                    {
                        pAt = ppElements[nTotalEles];
                    }
                    OnError(pAt);
                    return -1;
                }
            } //case         en_class_name: 
            break;
        case         en_class_name_risk:   //:
            {
                if (pTypes[nProcess2] == alphabet_type)
                {
                    if (0 == strcmp("virtual",ppElements[nProcess2]))
                    {
                        enLastState = en_class_derive_virtual;
                        ++nProcess2;
                    }
                    else
                    {
                        if (0 == strcmp("public",ppElements[nProcess2]))
                        {
                            ++nProcess2;
                        }
                        else if (0 == strcmp("protected",ppElements[nProcess2]))
                        {
                            ++nProcess2;
                        }
                        else if (0 == strcmp("private",ppElements[nProcess2]))
                        {
                            ++nProcess2;
                        }
                        if (pTypes[nProcess2] == alphabet_type)
                        {
                            enLastState = en_class_derive_class_name; //去掉名字
                            ++nProcess2;
                        }
                        else
                        {
                            printf("Incorrect line, a class name expected");
                            return -1;
                        }
                    }
                }
                else
                {
                    char *pAt =NULL;
                    if (nProcess2 < nTotalEles)
                    {
                        pAt = ppElements[nTotalEles];
                    }
                    OnError(pAt);
                    return -1;
                }
            } //case         en_class_name_risk:   //:
            break;
        case         en_class_derive_virtual://virtual 
            {
                if (pTypes[nProcess2] == alphabet_type)
                {
                    enLastState = en_class_derive_class_name;
                    ++nProcess2;
                }
                else
                {
                    char *pAt =NULL;
                    if (nProcess2 < nTotalEles)
                    {
                        pAt = ppElements[nTotalEles];
                    }
                    OnError(pAt);
                    return -1;
                }
            }  //case         en_class_derive_virtual://virtual 
            break;
        case         en_class_derive_class_name: //CDerive 
            {
                if (operator_type == pTypes[nProcess2])
                {
                    if (ppElements[nProcess2][0] == '{')
                    {
                        enLastState = en_class_right_brace;
                        ++nProcess2;
                    }
                    else
                    {
                        char *pAt =NULL;
                        if (nProcess2 < nTotalEles)
                        {
                            pAt = ppElements[nTotalEles];
                        }
                        OnError(pAt);
                        return -1;
                    }
                }
            } //
            break;
        case         en_class_right_brace:     //{ 这个时候需要的是一个type类型，或者 virtual \ pubic \ protected \private \ class \enum \struct \const \static \volatile \mutal 
            {
//                 int nRet = IgnorTypes(ppElements,pTypes,nProcess2,nTotalEles,2,enterAndDividerTypes); //
//                 nProcess2 += nRet;
                switch(pTypes[nProcess2])
                {
                case operator_type:
                    {
                        if (ppElements[nProcess2][0]=='}')//结束了
                        {
                            ++nProcess2;
//                             nRet = IgnorTypes(ppElements,pTypes,nProcess2,nTotalEles,2,enterAndDividerTypes); //
//                             nProcess2 += nRet;
                            if (semicolon_type == pTypes[nProcess2])
                            {
                                //if (ppElements[nProcess2][0]==';')//结束了
                                {
                                    ++nProcess2;
                                    return (nProcess2-nOld);
                                }
                            }
                            else
                            {
                                char *pAt =NULL;
                                if (nProcess2 < nTotalEles)
                                {
                                    pAt = ppElements[nTotalEles];
                                }
                                OnError(pAt);
                                return FAIL;
                            }
                        }
                        else if (ppElements[nProcess2][0]=='~')
                        {
                            //处理析构函数
                        }
                        else
                        {
                            char *pAt =NULL;
                            if (nProcess2 < nTotalEles)
                            {
                                pAt = ppElements[nTotalEles];
                            }
                            OnError(pAt);
                            return FAIL;
                        }
                       
                    }
                    break;
                case alphabet_type:
                    {
                        //if ()
                        CBaseElement *pEle = m_pOwner->IsKeyWords(ppElements[nProcess2]);
                        bool bNeedHandle = true;
                        if (pEle)
                        {
                            bNeedHandle = false;
                            switch (pEle->m_nElmentType)
                            {
                            case key_class:
                                {
                                    //新的 class
                                    CInterfaceElement *p = CREATE_FROM_STATIC_POOL(CInterfaceElement);
                                    if (p)
                                    {
                                        p->m_pFather = this;
                                        int nRet = p->Process(ppElements,pTypes,nProcess2,nTotalEles);
                                        if (nRet >=0)
                                        {
                                            AddType(p);
                                            nProcess2 += nRet;
                                        }
                                        else
                                        {
                                            return nRet;
                                        }
                                    }
                                    else
                                    {
                                        char *pAt =NULL;
                                        if (nProcess2 < nTotalEles)
                                        {
                                            pAt = ppElements[nTotalEles];
                                        }
                                        OnError(pAt);
                                        return OUT_OF_MEM; 
                                    }
                                    ++nProcess2;
                                }
                                break;
                            case key_namespace   :
                                {
                                    //不该有
                                    char *pAt =NULL;
                                    if (nProcess2 < nTotalEles)
                                    {
                                        pAt = ppElements[nTotalEles];
                                    }
                                    OnError(pAt);
                                    return FAIL;
                                }
                                break;
                            case key_struct      :
                                {
                                    //暂时不处理   
                                    char *pAt =NULL;
                                    if (nProcess2 < nTotalEles)
                                    {
                                        pAt = ppElements[nTotalEles];
                                    }
                                    OnError(pAt);
                                    return FAIL;
                                }
                                break;
                            case key_public      :
                            case key_protected   :
                            case key_private     :
                                {
                                    ++nProcess2;
                                    if (operator_type == pTypes[nProcess2])
                                    {
                                        if (':' !=ppElements[nProcess2][0])
                                        {
                                            char *pAt =NULL;
                                            if (nProcess2 < nTotalEles)
                                            {
                                                pAt = ppElements[nTotalEles];
                                            }
                                            OnError(pAt);
                                            return -1;
                                        }
                                        else
                                        {
                                            ++nProcess2;
                                        }
                                    }
                                }
                                break;
                            case key_virtual:
                                {
                                    ++nProcess2;
                                    //后面肯定是一个函数
                                    if (HandleAStatement(ppElements,pTypes,nProcess2,nTotalEles) <= SUCCESS)
                                    {
                                        char *pAt =NULL;
                                        if (nProcess2 < nTotalEles)
                                        {
                                            pAt = ppElements[nTotalEles];
                                        }
                                        OnError(pAt);
                                        return -1;
                                    }
                                    else
                                    {
                                        CMethod *pMethod = dynamic_cast<CMethod *>(GetLastElement());
                                        if (pMethod)
                                        {
                                            //CParamerter *pPar = ( CParamerter *)pMethod->GetLastElement();
                                            pMethod->AddVirtualDeg();
                                        }
                                    }
                                }
                                break;
                            default:
                                {
                                    bNeedHandle = true;
                                }
                            }
                        }
                        if (bNeedHandle) //没有
                        {
                            //处理构造函数和析构函数
                            int nRet = HandleAStatement(ppElements,pTypes,nProcess2,nTotalEles);
                            if (nRet < SUCCESS)
                            {
                                return nRet;
                            }
                        }
                    }
                }
            } //case         en_class_right_brace:
            break;
        }
    }
    
    if (semicolon_type == pTypes[nProcess2])
    {
        return nProcess2 - nOld + 1;
    }
    return 0;
}


int CInterfaceElement::HandleAStatement(char **ppElements,EnType *pTypes,int& nProcess2,int nTotalEles)
{
    CFullTypeDef *pFullType = CREATE_FROM_STATIC_POOL(CFullTypeDef);
    if (!pFullType)
    {
        char *pAt =NULL;
        if (nProcess2 < nTotalEles)
        {
            pAt = ppElements[nTotalEles];
        }
        OnError(pAt);
        return OUT_OF_MEM;
    }
    int nRet = pFullType->Process(ppElements,pTypes,nProcess2,nTotalEles);
    if (nRet > SUCCESS) //必须有
    {
        nProcess2 += nRet;
    }
    else
    {
        char *pAt =NULL;
        if (nProcess2 < nTotalEles)
        {
            pAt = ppElements[nTotalEles];
        }
        OnError(pAt);
        return nRet;
    }
    //find a name
    if (nProcess2 >= nTotalEles)
    {
        char *pAt =NULL;
        if (nProcess2 < nTotalEles)
        {
            pAt = ppElements[nTotalEles];
        }
        OnError(pAt);
        return INCORRECT_END;
    }
    if(alphabet_type == pTypes[nProcess2])
    {
        //a name;
        char *pName = ppElements[nProcess2];
        ++nProcess2;
        if (nProcess2 >= nTotalEles)
        {
            char *pAt =NULL;
            if (nProcess2 < nTotalEles)
            {
                pAt = ppElements[nTotalEles];
            }
            OnError(pAt);
            return INCORRECT_END;
        }
        if (blanket_type_1 == pTypes[nProcess2])
        {
            CMethod *pMethod = CREATE_FROM_STATIC_POOL(CMethod);
            if (!pMethod)
            {
                char *pAt =NULL;
                if (nProcess2 < nTotalEles)
                {
                    pAt = ppElements[nTotalEles];
                }
                OnError(pAt);
                return OUT_OF_MEM;
            }
            pMethod->SetName(pName);
            pMethod->InitFullName(pName);
            pMethod->SetRtnType(pFullType);
            nRet = pMethod->Process(ppElements,pTypes,nProcess2,nTotalEles);
            if (nRet > SUCCESS)
            {
                AddChildElement(pMethod);
                nProcess2 += nRet;
                return 1;
            }
            //肯定是
            OnError(ppElements[nProcess2]);
            return -1;
        }
        else
        {
            //处理数组？ 
            printf("Find a menber of the class!");
            while(semicolon_type != pTypes[nProcess2])
            {
                ++nProcess2;
                if (nProcess2 >= nTotalEles)
                {
                    char *pAt =NULL;
                    if (nProcess2 < nTotalEles)
                    {
                        pAt = ppElements[nTotalEles];
                    }
                    OnError(pAt);
                    return INCORRECT_END;
                }
            }
            CParamerter *pPar = CREATE_FROM_STATIC_POOL(CParamerter);
            if (!pPar)
            {
                OnError(ppElements[nProcess2]);
                return OUT_OF_MEM;
            }
            ++nProcess2; //处理一个
            pPar->SetType(pFullType);
            pPar->SetName(pName);
            AddChildElement(pPar);
            //忽略成員.
            return 0;
        }
    }
    else
    {
        char *pAt =NULL;
        if (nProcess2 < nTotalEles)
        {
            pAt = ppElements[nTotalEles];
        }
        OnError(pAt);
        return -1;
    }
}

const char *CInterfaceElement::GetHierachyName(void)
{
    return m_szName.c_str();
}


TInt32 CInterfaceElement::GenerateSkeleton(const char *pPath)
{
    int nFileNr = 0;
    int nRet = GenerateSkeletonHeaderFile(pPath);
    if (nRet > SUCCESS)
    {
        ++nFileNr;
    }
    nRet = GenerateSkeletonSourceFile(pPath);
    if (nRet > SUCCESS)
    {
        ++nFileNr;
    }
    //一个skeleton\stub

    return nFileNr;
}

TInt32 CInterfaceElement::GenerateSkeletonHeaderFile(const char *pPath)
{

    //生成一个skeleton , 一个stub.
    int nRet = 0;
    //stub 名字
    std::string szFileName = pPath;
    int nPathLen = szFileName.size();
    if (szFileName[nPathLen-1]=='/')
    {
        szFileName +="include/";
    }
    else
    {
        szFileName +="/include/";
    }
    szFileName += m_szName;
    szFileName += "Skeleton.h";
    FILE *pFile = fopen(szFileName.c_str(),"w");
    int nLength = 2*1024*1024;
    char *pBuff = NULL;
    NEW(pBuff,char,nLength);
    if (!pBuff)
    {
        return OUT_OF_MEM;
    }
    int nUsed = 0;
    if (pFile)
    {
        int nBegin = 10000;
        int n = sprintf_s(pBuff+nBegin,1000,"__%s_SKELETON_H__",m_szName.c_str());

        for (int i=0;i<n;++i)
        {
            pBuff[nBegin] = toupper(pBuff[nBegin]);
            ++nBegin;
        }
        n = sprintf_s(pBuff,nLength,"#ifndef %s\n#define %s\n",(pBuff+10000),(pBuff+10000));
        nUsed += n;
        nLength -= n;
        
        n = sprintf_s(pBuff+nUsed,nLength,"#include \"Public/include/Message.h\"\n#include \"../Interface/%s\"\n#include \"%sMethodId.h\"\n",CHeaderFile::m_pFileName,m_szName.c_str());
        nUsed +=n;
        nLength+=n;

        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCode(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }
        
        n = sprintf_s(pBuff+nUsed,nLength,"class %sSkeleton \n"
                                          "{\n"
                                          "public:\n"
                                          "    %s *m_pImplementObj;\n"
                                          "    %sSkeleton(%s *pIfObj)\n"
                                          "    {\n"
                                          "        m_pImplementObj = pIfObj;\n"
                                          "    }\n"
                                          "    TInt32 HandleMsg(CMessageHeader *pMsg);\n"
                                          "    static TBOOL  IsMine(CMessageHeader *pMsg) //是否属于这个接口\n"
                                          "     {\n"
                                          "         return (("
                                          ,m_szName.c_str(),m_szName.c_str(),m_szName.c_str(),m_szName.c_str());
                                          
                                          

        if (n<SUCCESS)
        {
            printf("Out of Mem!\n");
            return n;
        }

        nUsed +=n;
        nLength -= n;

        n = GetMethodIdStr(pBuff+nUsed,nLength);
        if (n<SUCCESS)
        {
            printf("Out of Mem!\n");
            return n;
        }
        nUsed +=n;
        nLength -=n;
        n = sprintf_s(pBuff+nUsed,nLength,")&(pMsg->GetMethodId()));\n     }\n");
        if (n<SUCCESS)
        {
            printf("Out of Mem!\n");
            return n;
        }
        nUsed +=n;
        nLength -= n;


        for (int i=0;i<m_tChilds.size();++i)
        {
            CBaseElement *pBase = m_tChilds[i].m_pPt;
            if (raw_method_type == pBase->m_nElmentType)
            {
                CMethod *pMethod = dynamic_cast<CMethod*>(pBase);
                n = sprintf_s(pBuff+nUsed,nLength,"    TInt32 ");
                nUsed += n;
                nLength -= n;
                
                n = pMethod->GetFullSkeletonMethodTxt(pBuff+nUsed,nLength);
                if (n < SUCCESS)
                {
                    return n;
                }
                else
                {
                    nUsed += n;
                    nLength -= n;
                }
                n = sprintf_s(pBuff+nUsed,nLength,";\n");
                if (n < SUCCESS)
                {
                    return n;
                }
                else
                {
                    nUsed += n;
                    nLength -= n;
                }
            }
        }
        
        n = sprintf_s(pBuff+nUsed,nLength,"};\n");
        if (n < SUCCESS)
        {
            return n;
        }
        else
        {
            nUsed += n;
            nLength -= n;
        }

        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCodeEnd(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }
        n = sprintf_s(pBuff+nUsed,nLength,"#endif\n");
        nUsed += n;
        nLength -= n;
        //sprintf_s()
        fwrite(pBuff,1,nUsed,pFile);
        //sprintf_s()
        fclose (pFile);
        delete [] pBuff;
        return SUCCESS;
    }
    //一个skeleton\stub

    return nRet;
}

TInt32 CInterfaceElement::GenerateSkeletonSourceFile(const char *pPath)
{
    int nRet = 0;
    //stub 名字
    std::string szFileName = pPath;
    int nPathLen = szFileName.size();
    if (szFileName[nPathLen-1]=='/')
    {
        szFileName +="source/";
    }
    else
    {
        szFileName +="/source/";
    }
    
    szFileName +=m_szName;
    szFileName += "Skeleton.cpp";
    FILE *pFile = fopen(szFileName.c_str(),"w");
    int nLength = 2*1024*1024;
    char *pBuff = NULL;
    NEW(pBuff,char,nLength);
    if (!pBuff)
    {
        return OUT_OF_MEM;
    }
    int nUsed = 0;
    if (pFile)
    {
        szFileName = "../include/";
        szFileName +=m_szName;
        szFileName += "Skeleton.h";

        int n = sprintf_s(pBuff,nLength,"#include \"%s\"\n",szFileName.c_str());
        nUsed += n;
        nLength -= n;

        n = sprintf_s(pBuff+nUsed,nLength,"#include \"Public/include/TypeUnmarshaller.h\"\n#include \"../include/%sMethodId.h\"\n",m_szName.c_str());
        nUsed += n;
        nLength -= n;

        //         n = sprintf_s(pBuff+nUsed,nLength,"public:\n",m_szName.c_str(),m_szName.c_str());
        //         nUsed += n;
        //         nLength -= n;
        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCode(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }
        // 
        //         n = sprintf_s(pBuff+nUsed,nLength,"class %sStub : public %s\n"
        //             "{\n"
        //             "public:\n"
        //             "IfSkeleton *m_pOnwerObj;\n"
        //             "CDoid  m_tTarget;\n"
        //             ,m_szName.c_str(),m_szName.c_str());
        //         nUsed += n;
        //         nLength -= n;
        // 

        //写处理函数
        n = sprintf_s(pBuff+nUsed,nLength,"TInt32 %sSkeleton::HandleMsg(CMessageHeader *pMsg)\n{\n",m_szName.c_str());
        if (n<SUCCESS)
        {
            return n;
        }
        else
        {
            nUsed += n;
            nLength -= n;
        }
        n = sprintf_s(pBuff+nUsed,nLength,"    typedef TInt32 (%sSkeleton::*_PFMSG)(CMessageHeader *); \n"
                                          "    struct _MSGMAP_ENTRY { TUInt32 m_uMsgID; _PFMSG m_pHandlerFunc; };\n"
                                          "    static _MSGMAP_ENTRY sMsgMapEntries[] = \n"
                                          "    {\n",m_szName.c_str());
        if (n<SUCCESS)
        {
            return n;
        }
        else
        {
            nUsed += n;
            nLength -= n;
        }
        for (int i=0;i<m_tChilds.size();++i)
        {
            CBaseElement *pBase = m_tChilds[i].m_pPt;
            if (raw_method_type == pBase->m_nElmentType)
            {
                CMethod *pMethod = dynamic_cast<CMethod*>(pBase);
                n = sprintf_s(pBuff+nUsed,nLength,"        ");
                nUsed += n;
                nLength -= n;
                n = pMethod->GetMethodFunPtTxt(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
                pBuff[nUsed] = ',';
                ++nUsed;
                --nLength;
                pBuff[nUsed] = '\n';
                ++nUsed;
                --nLength;
            }
        }
        
        n = sprintf_s(pBuff+nUsed,nLength,"    };\n"
                                      "    TInt32 nBegin = 0;\n"
                                      "    TInt32 nEnd = %u;\n"
                                      "    TUInt32 nMethodId = pMsg->GetMethodId();\n"
                                      "    _PFMSG pPfMsg = NULL;\n"
                                      "    while(nBegin < nEnd)\n"
                                      "    {\n"
                                      "        if (nBegin == (nEnd -1))\n"
                                      "        {\n"
                                      "            if (sMsgMapEntries[nBegin].m_uMsgID == nMethodId)\n"
                                      "            {\n"
                                      "                pPfMsg=sMsgMapEntries[nBegin].m_pHandlerFunc;\n"
                                      "            }\n"
                                      "            else if (sMsgMapEntries[nEnd].m_uMsgID == nMethodId)\n"
                                      "            {\n"
                                      "                pPfMsg=sMsgMapEntries[nEnd].m_pHandlerFunc;\n"
                                      "            }\n"
                                      "            break;\n"
                                      "        }\n"
                                      "        TInt32 nMiddle = (nBegin + nEnd) >> 1;\n"
                                      "        TUInt32 nMiddleVal = sMsgMapEntries[nMiddle].m_uMsgID;\n"
                                      "        if (nMiddleVal == nMethodId)\n"
                                      "        {\n"
                                      "            pPfMsg = sMsgMapEntries[nMiddle].m_pHandlerFunc;\n"
                                      "            break;\n"
                                      "        }\n"
                                      "        else\n"
                                      "        {\n"
                                      "            if (nMiddleVal>nMethodId)\n"
                                      "            {\n"
                                      "                nEnd = nMiddle;\n"
                                      "            }\n"
                                      "            else\n"
                                      "            {\n"
                                      "                nBegin   = nMiddle;\n"
                                      "            }\n"
                                      "        }\n"
                                      "    }\n"
                                      "    if (NULL == pPfMsg)\n"
                                      "    {\n"
                                      "        return CAN_NOT_HANDLE_THIS_MSG;\n"
                                      "    }\n"
                                      "    return (this->*pPfMsg)(pMsg);\n"
                                      "}; \n"
                                      ,m_tChilds.size());
        nUsed += n;
        nLength -= n;

        for (int i=0;i<m_tChilds.size();++i)
        {
            CBaseElement *p = m_tChilds[i].m_pPt;
            switch (p->m_nElmentType)
            {
            case raw_method_type:
                {
                    //只能加method
                    CMethod *pMethod = dynamic_cast<CMethod *>(p);
                    n = pMethod->GenerateSkeletonSourceCode(pBuff+nUsed,nLength);
                    if (n<SUCCESS)
                    {
                        return n;
                    }
                    else
                    {
                        nUsed += n;
                        nLength -= n;
                    }
                    //加一个回车
                    pBuff[nUsed]='\n';
                    ++nUsed;
                }
                break;
            }
        }

        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCodeEnd(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }

        fwrite(pBuff,1,nUsed,pFile);
        //sprintf_s()
        fclose (pFile);
        delete [] pBuff;
        pBuff = NULL;
        return nUsed;
    }
    //一个skeleton\stub
    return nRet;
}

TInt32 CInterfaceElement::GenerateStub(const char *pPath)
{
    //生成一个skeleton , 一个stub.
    int nFileNr = 0;
    int nRet = GenerateStubHeaderFile(pPath);
    if (nRet > SUCCESS)
    {
        ++nFileNr;
    }
    nRet = GenerateStubSourceFile(pPath);
    if (nRet > SUCCESS)
    {
        ++nFileNr;
    }
    //一个skeleton\stub

    return nFileNr;
}

TInt32 CInterfaceElement::GenerateMethodId(const char *pPath)
{
    return GenerateMethodIdFile(pPath,CBaseElement::sm_nInterfaceIdBegin++);
}


TInt32 CInterfaceElement::GenerateMethodIdFile(const char *pPath,int nInterfaceId)
{
    int nRet = 0;
    //stub 名字
    std::string szFileName = pPath;
    
    int nPathLen = szFileName.size();
    if (szFileName[nPathLen-1]=='/')
    {
        szFileName +="include/";
    }
    else
    {
        szFileName +="/include/";
    }
    szFileName += m_szName;
    szFileName += "MethodId.h";
    FILE *pFile = fopen(szFileName.c_str(),"w");
    int nLength = 2*1024*1024;
    char *pBuff = NULL;
    NEW(pBuff,char,nLength);
    if (!pBuff)
    {
        return OUT_OF_MEM;
    }
    int nUsed = 0;
    if (pFile)
    {
        int nBegin = 10000;
        int n = sprintf_s(pBuff+nBegin,1000,"__%s_METHOD_ID_H__",m_szName.c_str());

        for (int i=0;i<n;++i)
        {
            pBuff[nBegin] = toupper(pBuff[nBegin]);
            ++nBegin;
        }
        
        n = sprintf_s(pBuff,nLength,"#ifndef %s\n#define %s\n#include \"../../include/ServiceIdDef.h\"\n\n#define ",(pBuff+10000),(pBuff+10000));
        nUsed += n;
        nLength -= n;
        n = GetMethodIdStr(pBuff+nUsed,nLength);
        nUsed += n;
        nLength -= n;
        n = sprintf_s(pBuff+nUsed,nLength," (0x%08X) \n",nInterfaceId);
        nUsed += n;
        nLength -= n;

        int nMethodNr = 0;
        for (int i=0;i<m_tChilds.size();++i)
        {
            CBaseElement *pBase = m_tChilds[i].m_pPt;
            if (raw_method_type == pBase->m_nElmentType)
            {
                CMethod *pMethod = dynamic_cast<CMethod*>(pBase);
                n = sprintf_s(pBuff+nUsed,nLength,"#define ");
                nUsed += n;
                nLength -= n;
                n = pMethod->GetMethodIdStr(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
                n = sprintf_s(pBuff+nUsed,nLength," (0x%08X)\n",nMethodNr);
                ++nMethodNr;
                nUsed += n;
                nLength -= n;
            }
        }
        n = sprintf_s(pBuff+nUsed,nLength,"#endif\n\n");
        nUsed += n;
        nLength -= n;

        fwrite(pBuff,1,nUsed,pFile);
        //sprintf_s()
        fclose (pFile);
        delete [] pBuff;
        return nUsed;

    }
}

TInt32 CInterfaceElement::GenerateStubHeaderFile(const char *pPath)
{
    int nRet = 0;
    //stub 名字
    std::string szFileName = pPath;
    int nPathLen = szFileName.size();
    if (szFileName[nPathLen-1]=='/')
    {
        szFileName +="include/";
    }
    else
    {
        szFileName +="/include/";
    }
    szFileName += m_szName;
    szFileName += "Stub.h";
    FILE *pFile = fopen(szFileName.c_str(),"w");
    int nLength = 2*1024*1024;
    char *pBuff = NULL;
    NEW(pBuff,char,nLength);
    if (!pBuff)
    {
        return OUT_OF_MEM;
    }
    int nUsed = 0;
    if (pFile)
    {
        int nBegin = 10000;
        int n = sprintf_s(pBuff+nBegin,1000,"__%s_STUB_H__",m_szName.c_str());

        for (int i=0;i<n;++i)
        {
            pBuff[nBegin] = toupper(pBuff[nBegin]);
            ++nBegin;
        }

        n = sprintf_s(pBuff,nLength,"#ifndef %s\n#define %s\n#include \"Public/include/TypeDef.h\"\n#include \"Public/include/Doid.h\"\n#include \"Public/Interface/Platform/include/IfSkeleton.h\"\n",(pBuff+10000),(pBuff+10000));
        nUsed += n;
        nLength -= n;
        n = sprintf_s(pBuff+nUsed,nLength,"#include \"../Interface/%s\"\n",CHeaderFile::m_pFileName);
        nUsed +=n;
        nLength+=n;
//         n = sprintf_s(pBuff+nUsed,nLength,"#include \"%s\";\n",m_szName.c_str());
//         nUsed += n;
//         nLength -= n;

        //加namespace
        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCode(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }

        n = sprintf_s(pBuff+nUsed,nLength,"class %sStub : public %s\n"
            "{\n"
            "public:\n"
            "    IfSkeleton *m_pOnwerObj;\n"
            "    CDoid  m_tTarget;\n"
            "    void Init(IfSkeleton *pSkeleton,CDoid *pDoid)\n"
            "    {\n"
            "        m_pOnwerObj = pSkeleton;\n"
            "        m_tTarget = *pDoid;\n"
            "    }\n"
            ,m_szName.c_str(),m_szName.c_str());
        nUsed += n;
        nLength -= n;


        for (int i=0;i<m_tChilds.size();++i)
        {
            CBaseElement *p = m_tChilds[i].m_pPt;
            switch (p->m_nElmentType)
            {
            case raw_method_type:
                {
                    //只能加method
                    //sprintf_s(pBuff+nUsed,nLength,"%s %s(")
                    CMethod *pMethod = dynamic_cast<CMethod *>(p);
                    n = sprintf_s(pBuff+nUsed,nLength,"    ");
                    nUsed += n;
                    nLength -= n;
                    n = pMethod->GetFullMethodTxt(pBuff+nUsed,nLength);
                    if (n<SUCCESS)
                    {
                        return n;
                    }
                    else
                    {
                        nUsed += n;
                        nLength -= n;
                    }
                    //加一个回车
                    pBuff[nUsed]='\n';
                    ++nUsed;
                    --nLength;
                }
                break;
            }
        }
        n = sprintf_s(pBuff+nUsed,nLength,"};\n");
        nUsed += n;
        nLength -= n;

        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCodeEnd(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }
        n = sprintf_s(pBuff+nUsed,nLength,"#endif\n");
        nUsed += n;
        nLength -= n;

        fwrite(pBuff,1,nUsed,pFile);
        //sprintf_s()
        fclose (pFile);
        delete [] pBuff;
        return nUsed;
    }
    //一个skeleton\stub
    return nRet;
}
TInt32 CInterfaceElement::GenerateStubSourceFile(const char *pPath)
{
    int nRet = 0;
    //stub 名字
    std::string szFileName = pPath;
    int nPathLen = szFileName.size();
    if (szFileName[nPathLen-1]=='/')
    {
        szFileName +="source/";
    }
    else
    {
        szFileName +="/source/";
    }
    szFileName +=m_szName;
    szFileName += "Stub.cpp";
    FILE *pFile = fopen(szFileName.c_str(),"w");
    int nLength = 2*1024*1024;
    char *pBuff = NULL;
    NEW(pBuff,char,nLength);
    if (!pBuff)
    {
        return OUT_OF_MEM;
    }
    int nUsed = 0;
    if (pFile)
    {
        szFileName = "../include/";
        szFileName +=m_szName;
        szFileName += "Stub.h";

         int n = sprintf_s(pBuff,nLength,"#include \"%s\"\n",szFileName.c_str());
         nUsed += n;
         nLength -= n;

         n = sprintf_s(pBuff+nUsed,nLength,"#include \"Public/include/TypeMarshaller.h\"\n#include \"../include/%sMethodId.h\"\n",m_szName.c_str());
         nUsed += n;
         nLength -= n;
//         n = sprintf_s(pBuff+nUsed,nLength,"public:\n",m_szName.c_str(),m_szName.c_str());
//         nUsed += n;
//         nLength -= n;
        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCode(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }
// 
//         n = sprintf_s(pBuff+nUsed,nLength,"class %sStub : public %s\n"
//             "{\n"
//             "public:\n"
//             "IfSkeleton *m_pOnwerObj;\n"
//             "CDoid  m_tTarget;\n"
//             ,m_szName.c_str(),m_szName.c_str());
//         nUsed += n;
//         nLength -= n;
// 

        for (int i=0;i<m_tChilds.size();++i)
        {
            CBaseElement *p = m_tChilds[i].m_pPt;
            switch (p->m_nElmentType)
            {
            case raw_method_type:
                {
                    //只能加method
                    //sprintf_s(pBuff+nUsed,nLength,"%s %s(")
                    CMethod *pMethod = dynamic_cast<CMethod *>(p);
                    n = pMethod->GenerateStubSourceCode(pBuff+nUsed,nLength);
                    if (n<SUCCESS)
                    {
                        return n;
                    }
                    else
                    {
                        nUsed += n;
                        nLength -= n;
                    }
                    //加一个回车
                    pBuff[nUsed]='\n';
                    ++nUsed;
                }
                break;
            }
        }

        if (m_pFather)
        {
            if (raw_namespace_type == m_pFather->m_nElmentType)
            {
                CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
                n = pNS->GenerateNamespaceCodeEnd(pBuff+nUsed,nLength);
                nUsed += n;
                nLength -= n;
            }
        }

        fwrite(pBuff,1,nUsed,pFile);
        //sprintf_s()
        fclose (pFile);
        delete [] pBuff;
        pBuff = NULL;
        return nUsed;
    }
    //一个skeleton\stub
    return nRet;
}

TInt32 CInterfaceElement::GetMethodIdStr(char *pBuff,int nLength)
{
    int nRet = sprintf_s(pBuff,nLength,"%s_INTERFACE_ID",m_szName.c_str());
    for (int i=0;i<nRet;++i)
    {
        pBuff[i] = toupper(pBuff[i]);
    }
    return nRet;
}

}
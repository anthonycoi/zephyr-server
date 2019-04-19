#include "../include/nameSpace.h"
#include "../include/interfaceElement.h" 

namespace Zephyr
{
IMPLEMENT_STATIC_CLASS_POOL(CNamespace)

int CNamespace::findAType(char **ppWords,EnType *pTypes,int nProcess2,int nTotalEles)
{
    int nUsed=0;
    enum EnPreType
    {
        en_no_pre_type   = 0,
        en_const_type    = 1,
        en_static_type   = 2,
        en_volatile_type = 3,
    };
    enum EnState
    {
        en_begin_find_pretype,
        en_finding_type_name,
        en_found_name,
    };
    EnPreType preType = en_no_pre_type;
    EnState enState = en_begin_find_pretype;
    string szTypeName;
    int nNrOfWords = 0;
    for(int i=nProcess2;i<nTotalEles;++i)
    {
        switch (enState)
        {
        case en_begin_find_pretype:
            {
                if (alphabet_type == pTypes[i])
                {
                    //[const|volatile|static]+
                    if (0==strcmp("const",ppWords[i]))
                    {
                        preType = en_const_type;
                        enState = en_finding_type_name;
                        ++nNrOfWords;
                        break;
                    }
                    else if (0==strcmp("volatile",ppWords[i]))
                    {
                        preType = en_volatile_type;
                        enState = en_finding_type_name;
                        ++nNrOfWords;
                        break;
                    }
                    else if (0==strcmp("static",ppWords[i]))
                    {
                        preType = en_static_type;
                        enState = en_finding_type_name;
                        ++nNrOfWords;
                        break;
                    }
                }
                else
                {
                    printf("Incorrect Type!");
                    return -1;
                }
            }
        case en_finding_type_name:
            {
                if (alphabet_type == pTypes[i])
                {
                    szTypeName += ppWords[i];
                    ++nNrOfWords;
                    enState = en_found_name;
                }
                else
                {
                    printf("incorrect type");
                    return -1;
                }
            }
            break;
        case en_found_name:
            {
                if (alphabet_type == pTypes[i])
                {
                    
                    return nNrOfWords;
                }
                else if (operator_type == pTypes[i])
                {
                    if (0 == strcmp("::",ppWords[i]))
                    {
                        enState = en_finding_type_name;
                    }
                    else
                    {
                        return nNrOfWords;
                    }
                }
            }
            break;
        }
    }
    return nNrOfWords;
}
int CNamespace::findAComma(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles) //,
{
    if (operator_type == pTypes[nProcess2])
    {
        if (0==strcmp(",",ppElements[nProcess2]))
        {
            return 1;
        }
    }
    return -1;
}
int CNamespace::findAName(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles) //name;
{
    if (alphabet_type == pTypes[nProcess2])
    {
        return 1;
    }
    return -1;
}
int CNamespace::findTypeOpr(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles)//[*|**|*&|&|**];
{
    int nNr = 0;
    if (star_mark_type == pTypes[nProcess2])
    {
        ++nNr;
        while((nProcess2+nNr)<nTotalEles)
        {
            if(star_mark_type == pTypes[nProcess2+nNr])
            {
                ++nNr;
            }
            else
            {
                break;
            }
        }
        if (operator_type == pTypes[nProcess2+nNr])
        {
            if (0 == strcmp("&",ppElements[nProcess2+nNr+1]))
            {
                ++nNr;
            }
        }
    }
    return nNr;
}
int CNamespace::findMethod(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles) //[(+[Type+[*|&]+[NAME+[=[Num|Str|Char]]]]+[,+TYPE+[&|*]+[NAME]])];
{
    int nNr = 0;
    if (blanket_type_1 == pTypes[nProcess2])
    {
        ++nNr;
        while((nProcess2 +nNr)<nTotalEles)
        {
            if (blanket_type_2 == pTypes[nProcess2+nNr])
            {
                ++nNr;
                break;
            }
            ++nNr;
        }
    }
    return nNr;
}
int CNamespace::findADefaultValue(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles) //[=[Num|Str|Char]];
{
    int nNr = 0;
    if (operator_type == pTypes[nProcess2])
    {
        if ('='==ppElements[nProcess2][0])
        {
            //find!
            ++nNr;
            while((nProcess2+nNr)<nTotalEles)
            {
                if ((operator_type == pTypes[nProcess2+nNr])||(star_mark_type ==  pTypes[nProcess2+nNr]))
                {
                    ++nNr;
                }
                else
                {
                    break;
                }
            }
            if ((alphabet_type == pTypes[nProcess2+nNr])||(num_type==pTypes[nProcess2+nNr])||(const_char == pTypes[nProcess2+nNr])||(const_string == pTypes[nProcess2+nNr]))
            {
                ++nNr;
                return nNr;
            }
            else
            {
                printf("incorrect format!");
                return -1;
            }
        }
    }
    return 0;
}

TInt32 CNamespace::Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles)
{
    int nNr = 0;
    if (alphabet_type != pTypes[nProcess2])
    {
        printf ("Incorrect format");
        return -1;
    }
    else
    {
        m_szName = ppElements[nProcess2];
    }
    ++nNr;
    
    if (IsBracesBegin(ppElements[nProcess2+nNr],pTypes[nProcess2+nNr]))
    {
        ++nNr;
        while((!IsBracesEnd(ppElements[nProcess2+nNr],pTypes[nProcess2+nNr]))&&((nProcess2+nNr)<nTotalEles))
        {
            CBaseElement *pBase = IsKeyWords(ppElements[nProcess2+nNr]);
            if (pBase)
            {
                ++nNr;
                switch (pBase->m_nElmentType)
                {
                case key_class:
                    {
                        CInterfaceElement *pTmp  = CREATE_FROM_STATIC_POOL(CInterfaceElement);

                        if (pTmp)
                        {
                            int nRet = pTmp->Process(ppElements,pTypes,(nProcess2+nNr),nTotalEles);
                            if (nRet >= 0)
                            {
                                nNr += nRet;
                                AddChildElement(pTmp);
                                continue;
                            }
                            else
                            {
                                return -1;
                            }
                        }
                        else
                        {
                            return -1;
                        }
                    }
                    break;
                case key_namespace:
                    {
                        CNamespace *pTmp  = CREATE_FROM_STATIC_POOL(CNamespace);

                        if (pTmp)
                        {
                            int nRet = pTmp->Process(ppElements,pTypes,(nProcess2+nNr),nTotalEles);
                            if (nRet >= 0)
                            {
                                nNr += nRet;
                                AddChildElement(pTmp);
                                continue;
                            }
                            else
                            {
                                return -1;
                            }
                        }
                        else
                        {
                            return -1;
                        }
                    }
                    break;
                case key_struct:
                    {

                    }
                    break;
                }
            }
            int n = findAType(ppElements,pTypes,nProcess2+nNr,nTotalEles);
            if (n<=0)
            {
                printf("incorrect format!");
                return -1;
            }
            else
            {
                nNr += n;
            }

            n = findAType(ppElements,pTypes,nProcess2+nNr,nTotalEles);
            if (n<0)
            {
                printf("incorrect format!");
                return -1;
            }
            else
            {
                nNr += n;
            }
        
            n = findAName(ppElements,pTypes,nProcess2+nNr,nTotalEles);
            if (n<=0)
            {
                printf("incorrect format!");
                return -1;
            }
            else
            {
                nNr += n;
            }

            n = findMethod(ppElements,pTypes,nProcess2+nNr,nTotalEles);
            if (n<0)
            {
                printf("incorrect format!");
                return -1;
            }
            else
            {
                nNr += n;
            }
            
            if (operator_type==pTypes[nProcess2+nNr])
            {
                if (';'==ppElements[nProcess2+nNr][0])
                {
                    ++nNr;
                    continue;;
                }
            }
            printf("incorrect type");
            return -1;
        }
        ++nNr;
        return nNr;
    }
    else
    {
        return -1;
    }
}

TInt32 CNamespace::GenerateSkeleton(const char *pPath)
{
    int n = m_tChilds.size();
    int nTotalSize = 0;
    for(int i=0;i<n;++i)
    {
        CBaseElement *p = m_tChilds[i].m_pPt;
        switch (p->m_nElmentType)
        {
        case raw_interface_type:
        case raw_namespace_type:
        case raw_method_type:
            {
                int nRet = p->GenerateSkeleton(pPath);
                if (nRet < SUCCESS)
                {
                    nTotalSize += nRet;
                }
            }
            break;
        default:
            {

            }
        }
    }
    return nTotalSize;
}

TInt32 CNamespace::GenerateStub(const char *pPath)
{
    int n = m_tChilds.size();
    int nTotalSize = 0;
    for(int i=0;i<n;++i)
    {
        CBaseElement *p = m_tChilds[i].m_pPt;
        switch (p->m_nElmentType)
        {
        case raw_interface_type:
        case raw_namespace_type:
        case raw_method_type:
            {
                int nRet = p->GenerateStub(pPath);
                if (nRet < SUCCESS)
                {
                    nTotalSize += nRet;
                }
            }
            break;
        default:
            {

            }
        }
    }
    return nTotalSize;
}

TInt32 CNamespace::GenerateNamespaceCode(char *pBuff,int nLength)
{
    int n = 0;
    int nUsed = 0;
    if (m_pFather)
    {
        if (raw_namespace_type == m_pFather->m_nElmentType)
        {
            CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
            if (pNS)
            {
                n = pNS->GenerateNamespaceCode(pBuff,nLength);
                nUsed += n;
                nLength -= n;
            }
        }
    }
    n = sprintf_s(pBuff+nUsed,nLength,"namespace %s \n{\n",m_szName.c_str());
    nUsed += n;
    nLength -= n;
    return nUsed;
}

TInt32 CNamespace::GenerateNamespaceCodeEnd(char *pBuff,int nLength)
{
    int n = 0;
    int nUsed = 0;
    if (m_pFather)
    {
        if (raw_namespace_type == m_pFather->m_nElmentType)
        {
            CNamespace *pNS = dynamic_cast<CNamespace *>(m_pFather);
            if (pNS)
            {
                n = pNS->GenerateNamespaceCodeEnd(pBuff,nLength);
                nUsed += n;
                nLength -= n;
            }
        }
    }
    n = sprintf_s(pBuff+nUsed,nLength,"}\n");
    nUsed += n;
    nLength -= n;
    return nUsed;
}

}


#include "../include/FullTypeClass.h"
#include <algorithm>
namespace Zephyr
{

    
IMPLEMENT_STATIC_CLASS_POOL(CFullTypeDef)

void CFullTypeDef::AddPrefix(EnPrefix enPrefix)
{
    if (!(m_uPrefix & enPrefix))
    {
        m_uPrefix |= enPrefix;
        switch (enPrefix)
        {
        case en_const_prefix:
            {
                m_szFull += "const_";
                m_szRawTxt += "const ";
            }
            break;
        case en_static_prefix:
            {
                m_szFull += "static_";
                m_szRawTxt += "static ";
            }
            break;
        case en_mutable_prefix:
            {
                m_szFull += "mutable_";
                m_szRawTxt += "mutable ";
            }
            break;
        case en_volatile_prefix:
            {
                m_szFull += "volatile_";
                m_szRawTxt += "volatile ";
            }
            break;
        }
    }
}


CFullTypeDef::CFullTypeDef()
{
    m_uPrefix = 0;
    m_pType = NULL;
    m_szFull.clear();
    m_nElmentType = raw_full_type_type;
    for (int i=0;i<4;++i)
    {
        m_tOprs[i] = 0;
    }
}

void CFullTypeDef::AddOpr(EnOperator enOpr)
{
    for (int i=0;i<4;++i)
    {
        if (en_no_opr == m_tOprs[i])
        {
            switch (enOpr)
            {
            case en_star_operator: 
                {
                    m_szFull += "_pt";
                    m_szRawTxt += "*";
                    m_szRawNoPrefix += "*";
                    m_tOprs[i] = en_star_operator;
                }
                break;
            case en_ref_operator:
                {
                    m_szFull += "_ref";
                    m_szRawTxt += "&";
                    m_tOprs[i] = en_ref_operator;
                }
                break;
            }
            break;
        }
    }
}

TInt32 CFullTypeDef::Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles)
{
    int nOld = nProcess2;
    CBaseElement *pEle = IsKeyWords(ppElements[nProcess2]);
    while (pEle)
    {
        switch (pEle->m_nElmentType)
        {
        case key_static:
            {
                AddPrefix(en_static_prefix);
                ++nProcess2;
            }
            break;
        case key_const:
            {
                AddPrefix(en_const_prefix);
                ++nProcess2;
            }
            break;
        case key_volatile:
            {
                AddPrefix(en_volatile_prefix);
                ++nProcess2;
            }
            break;
        case key_mutable:
            {
                AddPrefix(en_mutable_prefix);
                ++nProcess2;       
            }
            break;
        default:
            {
                
            }
        }
        pEle = IsKeyWords(ppElements[nProcess2]);
    }
    
    //一个typename
    if (alphabet_type == pTypes[nProcess2])
    {
        //find a type
        m_szFull += ppElements[nProcess2];
        m_szRawTxt += ppElements[nProcess2];
        m_szRawNoPrefix += ppElements[nProcess2];
        ++nProcess2;
        //处理模板
        if (smaller_mark_type == pTypes[nProcess2])
        {
            //if ('<' == ppElements[nProcess2][0])
            m_szFull += "_tpl_begin_";
            m_szRawTxt += ppElements[nProcess2];
            m_szRawNoPrefix += ppElements[nProcess2];
            {
                ++nProcess2;
                //一个类型
                bool bMoreTplSubTypes = true;
                while(bMoreTplSubTypes)
                {
                    CFullTypeDef *pFullType = CREATE_FROM_STATIC_POOL(CFullTypeDef);
                    if (!pFullType)
                    {
                        return OUT_OF_MEM;
                    }
                    int n = pFullType->Process(ppElements,pTypes,nProcess2,nTotalEles);
                    if (n < SUCCESS)
                    {
                        return n;
                    }
                    m_szFull += pFullType->m_szFull;
                    m_szRawTxt += pFullType->m_szRawTxt;
                    m_szRawNoPrefix += pFullType->m_szRawTxt;

                    nProcess2 += n;
                    m_pTplSubs.push_back(pFullType);
                    if (operator_type == pTypes[nProcess2])
                    {
                        if (','==ppElements[nProcess2][0])
                        {
                            m_szFull += "_and_";
                            m_szRawTxt += ",";
                            m_szRawNoPrefix += ",";

                            bMoreTplSubTypes = true;
                            nProcess2++;
                        }
                        else
                        {
                            bMoreTplSubTypes = false;
                        }
                    }
                    else
                    {
                        bMoreTplSubTypes = false;
                    }
                }
                
                if (bigger_mark_type == pTypes[nProcess2])
                {
                    if ('>' == ppElements[nProcess2][0])
                    {
                        m_szFull += "_tpl_end_";
                        m_szRawTxt += ppElements[nProcess2];
                        m_szRawNoPrefix += ppElements[nProcess2];
                        ++nProcess2;
                    }
                }
                else
                {
                    printf("can not found the template's end \'>\' \n\r");
                    return -1;
                }
            }
        }
        CBaseElement *pFoundType = IsOneType(m_szRawNoPrefix.c_str());
        if (!pFoundType)
        {
            pFoundType = CREATE_FROM_STATIC_POOL(CBaseElement);
            if (!pFoundType)
            {
                printf("Out of Mem");

                return OUT_OF_MEM;
            }
            pFoundType->SetName(m_szRawNoPrefix.c_str());
            AddType(pFoundType);
        }
        m_pType = pFoundType;
    }
    while((operator_type == pTypes[nProcess2])||(star_mark_type == pTypes[nProcess2]))
    {
        int n = 0;
        while(0 != ppElements[nProcess2][n])
        {
            switch(ppElements[nProcess2][n])
            {
            case '*':
                {
                    AddOpr(en_star_operator);
                }
                break;
            case '&':
                {
                    AddOpr(en_ref_operator);
                }
                break;
            case ',': //为模板类特加的.
                {
                    return (nProcess2 - nOld);
                }
            default:
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
            ++n;
        }
        ++nProcess2;
    }
    return (nProcess2 - nOld);
}

const char *CFullTypeDef::GetHierachyName()
{
    return m_szFull.c_str();
}


}
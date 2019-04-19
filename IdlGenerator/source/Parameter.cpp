#include "../include/Parameter.h"

namespace Zephyr
{

IMPLEMENT_STATIC_CLASS_POOL(CParamerter)

TInt32 CParamerter::Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles)
{
    //先找到一个fullType,
    int nOld = nProcess2;
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
    if (nRet>SUCCESS)
    {
        nProcess2 += nRet;
    }
    else
    {
        return nRet;
    }
    if (alphabet_type == pTypes[nProcess2])
    {
        SetName(ppElements[nProcess2]);
        ++nProcess2;
    }
    else
    {
        static int nName = 0;
        char szBuff[16];
        sprintf(szBuff,"nonName%d",szBuff);
        SetName(szBuff);
    }
    m_pFullType = pFullType;
    return (nProcess2-nOld);
    //再找到一个名字
}

}